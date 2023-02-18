#pragma once

#include <utility>

#include "audio_param.h"
#include "cmd_handling.h"
#include "messages.h"
#include "midi_mapping_json_builder.h"
#include "midi_mapping_json_parser.h"
#include "midi_uart.h"
#include "os/queue.h"
#include "os/timer.h"
#include "persistence.h"

namespace shrapnel {

namespace {
constexpr const char *TAG = "main_thread";
}

constexpr const size_t MAX_PARAMETERS = 20;

using AudioParameters = parameters::AudioParameters<MAX_PARAMETERS, 1>;
using SendMessageCallback = etl::delegate<void(const AppMessage &)>;

// TODO move all the json parser function into a json namespace
namespace midi {
using midi::from_json;

template <>
std::optional<float> from_json(const rapidjson::Value &value)
{
    if(!value.IsFloat())
    {
        ESP_LOGE(TAG, "not float");
        return std::nullopt;
    }
    return value.GetFloat();
}

template <>
rapidjson::Value to_json(rapidjson::Document &document, const float &object)
{
    rapidjson::Value out{};
    out.SetFloat(object);
    return out;
}
} // namespace midi

template <typename MappingManagerT>
class MidiMappingObserver final : public midi::MappingObserver
{
public:
    explicit MidiMappingObserver(persistence::Storage &persistence,
                                 const MappingManagerT &mapping_manager)
        : persistence{persistence},
          mapping_manager{mapping_manager}
    {
    }

    void notification(const midi::Mapping::id_t &) override
    {
        ESP_LOGI(TAG, "Midi mapping has changed");
        persist();
    }

private:
    void persist()
    {
        rapidjson::Document document;

        auto mapping_data = midi::to_json(document, *mapping_manager.get());
        document.Swap(mapping_data);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer{buffer};
        document.Accept(writer);

        persistence.save("midi_mappings", buffer.GetString());
    };

    persistence::Storage &persistence;
    const MappingManagerT &mapping_manager;
};

class ParameterUpdateNotifier
{
public:
    ParameterUpdateNotifier(std::shared_ptr<AudioParameters> audio_params,
                            SendMessageCallback send_message)
        : audio_params{std::move(audio_params)},
          send_message{send_message}
    {
    }

    int update(const parameters::id_t &param, float value)
    {
        auto message = AppMessage{
            parameters::Update{
                param,
                value,
            },
            std::nullopt,
        };
        send_message(message);
        return audio_params->update(param, value);
    }

    float get(const parameters::id_t &param)
    {
        return audio_params->get(param);
    }

private:
    std::shared_ptr<AudioParameters> audio_params;
    SendMessageCallback send_message;
};

template <std::size_t MAX_PARAMETERS>
class ParameterObserver final : public parameters::ParameterObserver
{
public:
    explicit ParameterObserver(persistence::Storage &persistence)
        : persistence{persistence},
          timer{"param save throttle",
                pdMS_TO_TICKS(10'000),
                false,
                etl::delegate<void(void)>::create<
                    ParameterObserver<MAX_PARAMETERS>,
                    &ParameterObserver<MAX_PARAMETERS>::timer_callback>(*this)}
    {
    }

    void timer_callback()
    {
        is_save_throttled.clear();
        is_save_throttled.notify_all();
    }

    void
    notification(std::pair<const parameters::id_t &, float> parameter) override
    {
        auto &[id, value] = parameter;
        ESP_LOGI(
            TAG, "notified about parameter change %s %f", id.data(), value);
        if(!updated_parameters.available())
        {
            ESP_LOGE(TAG, "no space available");
            return;
        }

        updated_parameters[id] = value;

        if(!timer.is_active())
        {
            timer.start(pdMS_TO_TICKS(5));
        }
    }

    void persist_parameters()
    {
        for(const auto &param : updated_parameters)
        {
            rapidjson::Document document;

            auto parameter_data = midi::to_json(document, param.second);
            document.Swap(parameter_data);

            rapidjson::StringBuffer buffer;
            rapidjson::Writer writer{buffer};
            document.Accept(writer);

            persistence.save(param.first.data(), buffer.GetString());
        }

        updated_parameters.clear();
    }

    std::atomic_flag is_save_throttled;

private:
    persistence::Storage &persistence;
    os::Timer timer;
    etl::map<parameters::id_t, float, MAX_PARAMETERS> updated_parameters;
};

template <std::size_t MAX_PARAMETERS, std::size_t QUEUE_LEN>
class MainThread
{
public:
    MainThread(SendMessageCallback send_message,
               Queue<AppMessage, QUEUE_LEN> &in_queue,
               midi::MidiUartBase *midi_uart,
               std::shared_ptr<AudioParameters> audio_params,
               persistence::Storage &persistence)
        : send_message{send_message},
          in_queue{in_queue},
          parameter_observer{persistence},
          clipping_throttle_timer{
              "clipping throttle", pdMS_TO_TICKS(1000), false},
          midi_message_notify_timer{
              "midi notify",
              pdMS_TO_TICKS(100),
              true,
              os::Timer::Callback::
                  create<MainThread, &MainThread::clear_midi_notify_waiting>(
                      *this)},
          midi_uart{midi_uart},
          last_midi_message{},
          last_notified_midi_message{},
          midi_decoder{std::make_unique<midi::Decoder>(
              midi::Decoder::Callback::create<MainThread,
                                              &MainThread::on_midi_message>(
                  *this))},
          midi_mutex{},
          audio_params{audio_params},
          cmd_handling{
              std::make_unique<parameters::CommandHandling<AudioParameters>>(
                  audio_params,
                  parameters::CommandHandling<AudioParameters>::
                      SendMessageCallback::create<
                          MainThread,
                          &MainThread::cmd_handling_send_message>(*this))}
    {
        auto create_and_load_parameter = [&](const parameters::id_t &name,
                                             float minimum,
                                             float maximum,
                                             float default_value)
        {
            std::optional<float> loaded_value;
            etl::string<128> json_string{};
            int rc = persistence.load(name.data(), json_string);
            if(rc != 0)
            {
                ESP_LOGW(TAG, "Parameter %s failed to load", name.data());
                goto out;
            }

            {
                rapidjson::Document document;
                document.Parse(json_string.data());

                if(!document.HasParseError())
                {
                    loaded_value = midi::from_json<float>(document);
                }
                else
                {
                    ESP_LOGE(TAG,
                             "document failed to parse '%s'",
                             json_string.data());
                }
            }

        out:
            auto range = maximum - minimum;
            rc = audio_params->create_and_add_parameter(
                name,
                minimum,
                maximum,
                loaded_value.has_value() ? *loaded_value * range + minimum
                                         : default_value);
            if(rc != 0)
            {
                ESP_LOGE(TAG, "Failed to create parameter %s", name.c_str());
            }
        };

        // XXX: These are duplicated in the JUCE plugin, be sure to update both at
        // the same time
        create_and_load_parameter("ampGain", 0, 1, 0.5);
        create_and_load_parameter("ampChannel", 0, 1, 0);
        create_and_load_parameter("bass", 0, 1, 0.5);
        create_and_load_parameter("middle", 0, 1, 0.5);
        create_and_load_parameter("treble", 0, 1, 0.5);
        //contour gets unstable when set to 0
        create_and_load_parameter("contour", 0.01, 1, 0.5);
        create_and_load_parameter("volume", -30, 0, -15);

        create_and_load_parameter("noiseGateThreshold", -80, 0, -60);
        create_and_load_parameter("noiseGateHysteresis", 0, 5, 0);
        create_and_load_parameter("noiseGateAttack", 1, 50, 10);
        create_and_load_parameter("noiseGateHold", 1, 250, 50);
        create_and_load_parameter("noiseGateRelease", 1, 250, 50);
        create_and_load_parameter("noiseGateBypass", 0, 1, 0);

        create_and_load_parameter("chorusRate", 0.1, 4, 0.95);
        create_and_load_parameter("chorusDepth", 0, 1, 0.3);
        create_and_load_parameter("chorusMix", 0, 1, 0.8);
        create_and_load_parameter("chorusBypass", 0, 1, 1);

        create_and_load_parameter("wahPosition", 0, 1, 0.5);
        create_and_load_parameter("wahVocal", 0, 1, 0);
        create_and_load_parameter("wahBypass", 0, 1, 1);

        audio_params->add_observer(parameter_observer);

        auto parameter_notifier = std::make_shared<ParameterUpdateNotifier>(
            audio_params, send_message);

        [&]
        {
            /* TODO How to reduce the memory usage?
           * - We could store each entry in the table at a different key
           * - Use the streaming API so that the entire message does not have to
           *   be parsed at once?
           * - Replace etl::map with more efficient implementation
           */
            etl::string<1500> mapping_json;
            persistence.load("midi_mappings", mapping_json);
            ESP_LOGI(TAG, "saved mappings: %s", mapping_json.c_str());

            rapidjson::Document document;
            document.Parse(mapping_json.c_str());
            auto saved_mappings = midi::from_json<
                etl::map<midi::Mapping::id_t, midi::Mapping, 10>>(document);

            using MidiMappingType = midi::MappingManager<ParameterUpdateNotifier, 10, 1>;
            midi_mapping_manager =
                saved_mappings.has_value()
                    ? std::make_unique<MidiMappingType>(parameter_notifier,
                                                        *saved_mappings)
                    : std::make_unique<MidiMappingType>(parameter_notifier);
        }();

        mapping_observer = std::make_unique<MidiMappingObserver<
            midi::MappingManager<ParameterUpdateNotifier, 10, 1>>>(
            persistence, *midi_mapping_manager);
        midi_mapping_manager->add_observer(*mapping_observer);

        midi_message_notify_timer.start(portMAX_DELAY);

        events::input_clipped.test_and_set();
        events::output_clipped.test_and_set();
    }

    void loop()
    {
        if(auto byte = midi_uart->get_byte(0); byte.has_value())
        {
            ESP_LOGI(TAG, "midi got byte 0x%02x", *byte);

            midi_decoder->decode(*byte);
        }

        if(AppMessage message; in_queue.receive(&message, 0))
        {
            auto fd = message.second;

            std::visit(
                [&](const auto &message)
                {
                    using T = std::decay_t<decltype(message)>;

                    if constexpr(std::is_same_v<T, parameters::ApiMessage>)
                    {
                        if(!fd.has_value())
                        {
                            ESP_LOGE(TAG, "Must always have fd");
                        }

                        cmd_handling->dispatch(message, *fd);
                    }
                    else if constexpr(std::is_same_v<T,
                                                     midi::MappingApiMessage>)
                    {
                        std::scoped_lock lock{midi_mutex};

                        auto response = std::visit(
                            [&](const auto &message)
                                -> std::optional<midi::MappingApiMessage>
                            {
                                using T = std::decay_t<decltype(message)>;

                                if constexpr(std::is_same_v<T,
                                                            midi::GetRequest>)
                                {
                                    auto mappings = midi_mapping_manager->get();
                                    return midi::GetResponse{mappings};
                                }
                                else if constexpr(std::is_same_v<
                                                      T,
                                                      midi::CreateRequest>)
                                {
                                    auto rc = midi_mapping_manager->create(
                                        message.mapping);
                                    if(rc == 0)
                                    {
                                        return midi::CreateResponse{
                                            message.mapping};
                                    }
                                }
                                else if constexpr(std::is_same_v<T,
                                                                 midi::Update>)
                                {
                                    // return code ignored, as there is no way to indicate
                                    // the error to the frontend
                                    (void)midi_mapping_manager->update(
                                        message.mapping);
                                }
                                else if constexpr(std::is_same_v<T,
                                                                 midi::Remove>)
                                {
                                    midi_mapping_manager->remove(message.id);
                                }
                                else
                                {
                                    ESP_LOGE(TAG, "Unhandled message type");
                                }

                                return std::nullopt;
                            },
                            message);

                        if(response.has_value())
                        {
                            send_message({*response, std::nullopt});
                        }
                    }
                },
                message.first);
        }

        if(!clipping_throttle_timer.is_active())
        {
            if(!events::input_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "input was clipped");
                send_message({events::InputClipped{}, std::nullopt});
                clipping_throttle_timer.start(pdMS_TO_TICKS(10));
            }

            if(!events::output_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "output was clipped");
                send_message({events::OutputClipped{}, std::nullopt});
                clipping_throttle_timer.start(pdMS_TO_TICKS(10));
            }
        }

        if(!parameter_observer.is_save_throttled.test_and_set())
        {
            parameter_observer.persist_parameters();
            ESP_LOGI(TAG, "Parameters saved to NVS");
        }

        if(!is_midi_notify_waiting.test_and_set() &&
           last_midi_message.has_value() &&
           last_notified_midi_message != *last_midi_message)
        {
            last_notified_midi_message = *last_midi_message;

            send_message(
                {midi::MessageReceived{*last_midi_message}, std::nullopt});
        }
    }

private:
    void on_midi_message(midi::Message message)
    {
        etl::string<40> buffer;
        etl::string_stream stream{buffer};
        stream << message;
        ESP_LOGI(TAG, "%s", buffer.data());

        last_midi_message = message;

        {
            std::scoped_lock lock{midi_mutex};
            midi_mapping_manager->process_message(message);
        }
    };

    void clear_midi_notify_waiting() { is_midi_notify_waiting.clear(); };

    void cmd_handling_send_message(const parameters::ApiMessage &m,
                                   std::optional<int> fd)
    {
        send_message({m, fd});
    }

    SendMessageCallback send_message;
    Queue<AppMessage, QUEUE_LEN> &in_queue;
    ParameterObserver<MAX_PARAMETERS> parameter_observer;
    os::Timer clipping_throttle_timer;
    os::Timer midi_message_notify_timer;
    std::atomic_flag is_midi_notify_waiting;
    midi::MidiUartBase *midi_uart;
    std::optional<midi::Message> last_midi_message;
    std::optional<midi::Message> last_notified_midi_message;
    std::unique_ptr<midi::Decoder> midi_decoder;
    std::mutex midi_mutex;
    std::unique_ptr<midi::MappingManager<ParameterUpdateNotifier, 10, 1>>
        midi_mapping_manager;
    std::shared_ptr<AudioParameters> audio_params;
    std::unique_ptr<parameters::CommandHandling<
        parameters::AudioParameters<MAX_PARAMETERS, 1>>>
        cmd_handling;
    std::unique_ptr<MidiMappingObserver<
        midi::MappingManager<ParameterUpdateNotifier, 10, 1>>>
        mapping_observer;
};

} // namespace shrapnel