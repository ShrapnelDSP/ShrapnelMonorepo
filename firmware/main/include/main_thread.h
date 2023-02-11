// The code in this file has no dependencies on FreeRTOS or anything else in the
// ESP32 platform
#pragma once

#include <utility>

#include "audio_param.h"
#include "messages.h"
#include "midi_mapping_json_builder.h"
#include "midi_uart.h"
#include "persistence.h"
#include "server.h"
#include "timer.h"
#include "wifi_state_machine.h"

namespace shrapnel {

namespace {
constexpr const char *TAG = "main_thread";
}

constexpr const size_t MAX_PARAMETERS = 20;
using AudioParameters = parameters::AudioParameters<MAX_PARAMETERS, 1>;

class EventSend final
{
public:
    explicit EventSend(Server &server) : server{server} {}

    void send(parameters::ApiMessage message, int fd)
    {
        if(fd >= 0)
        {
            server.send_message({message, fd});
        }
        else
        {
            server.send_message({message, std::nullopt});
        }
    }

private:
    Server &server;
};

class ParameterUpdateNotifier
{
public:
    ParameterUpdateNotifier(std::shared_ptr<AudioParameters> audio_params,
                            Server &server)
        : audio_params{std::move(audio_params)}, server{server}
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
        server.send_message(message);
        return audio_params->update(param, value);
    }

    float get(const parameters::id_t &param)
    {
        return audio_params->get(param);
    }

private:
    std::shared_ptr<AudioParameters> audio_params;
    Server &server;
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
    MainThread(ParameterObserver<MAX_PARAMETERS> &parameter_observer,
               Queue<shrapnel::wifi::InternalEvent, 3> &wifi_queue,
               etl::istate_chart<void> &wifi_state_chart,
               os::Timer &clipping_throttle_timer,
               std::atomic_flag &is_midi_notify_waiting,
               midi::MidiUartBase *midi_uart,
               const std::optional<midi::Message> &last_midi_message,
               std::optional<midi::Message> &last_notified_midi_message,
               midi::Decoder *midi_decoder,
               Queue<AppMessage, QUEUE_LEN> *in_queue,
               parameters::CommandHandling<
                   parameters::AudioParameters<MAX_PARAMETERS, 1>,
                   EventSend> *cmd_handling,
               std::mutex &midi_mutex,
               midi::MappingManager<ParameterUpdateNotifier, 10, 1>
                   *midi_mapping_manager,
               Server &server)
        : parameter_observer{parameter_observer}, wifi_queue{wifi_queue},
          wifi_state_chart{wifi_state_chart},
          clipping_throttle_timer{clipping_throttle_timer},
          is_midi_notify_waiting{is_midi_notify_waiting}, midi_uart{midi_uart},
          last_midi_message{last_midi_message},
          last_notified_midi_message{last_notified_midi_message},
          midi_decoder{midi_decoder}, in_queue{in_queue},
          cmd_handling{cmd_handling}, midi_mutex{midi_mutex},
          midi_mapping_manager{midi_mapping_manager}, server{server}
    {
    }

    void loop()
    {
        if(auto byte = midi_uart->get_byte(0); byte.has_value())
        {
            ESP_LOGI(TAG, "midi got byte 0x%02x", *byte);

            midi_decoder->decode(*byte);
        }

        if(AppMessage message; in_queue->receive(&message, 0))
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
                            server.send_message({*response, std::nullopt});
                        }
                    }
                },
                message.first);
        }

        {
            // i2s produces an event on each buffer TX/RX. We process all the
            // events in the current iteration, so that the queue doesn't fill
            // up.
            i2s_event_t event;
            while(xQueueReceive(audio::i2s_queue, &event, 0))
            {
                audio::log_i2s_event(event);
            }
        }

        if(!clipping_throttle_timer.is_active())
        {
            if(!events::input_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "input was clipped");
                server.send_message({events::InputClipped{}, std::nullopt});
                clipping_throttle_timer.reset(pdMS_TO_TICKS(10));
            }

            if(!events::output_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "output was clipped");
                server.send_message({events::OutputClipped{}, std::nullopt});
                clipping_throttle_timer.reset(pdMS_TO_TICKS(10));
            }
        }

        wifi::InternalEvent wifi_event;
        while(pdPASS == wifi_queue.receive(&wifi_event, 0))
        {
            wifi::State state{wifi_state_chart.get_state_id()};
            ESP_LOGI(
                TAG, "state: %s event: %s", state.c_str(), wifi_event.c_str());

            wifi_state_chart.process_event(wifi_event);

            wifi::State new_state{wifi_state_chart.get_state_id()};
            if(new_state != state)
            {
                ESP_LOGI(TAG, "changed to state: %s", new_state.c_str());
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

            server.send_message(
                {midi::MessageReceived{*last_midi_message}, std::nullopt});
        }
    }

private:
    ParameterObserver<MAX_PARAMETERS> &parameter_observer;
    Queue<shrapnel::wifi::InternalEvent, 3> &wifi_queue;
    etl::istate_chart<void> &wifi_state_chart;
    os::Timer &clipping_throttle_timer;
    std::atomic_flag &is_midi_notify_waiting;
    midi::MidiUartBase *midi_uart;
    const std::optional<midi::Message> &last_midi_message;
    std::optional<midi::Message> &last_notified_midi_message;
    midi::Decoder *midi_decoder;
    Queue<AppMessage, QUEUE_LEN> *in_queue;
    parameters::CommandHandling<parameters::AudioParameters<MAX_PARAMETERS, 1>,
                                EventSend> *cmd_handling;
    std::mutex &midi_mutex;
    midi::MappingManager<ParameterUpdateNotifier, 10, 1> *midi_mapping_manager;
    Server &server;
};

} // namespace shrapnel