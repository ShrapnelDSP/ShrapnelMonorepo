/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <midi_mapping.h>
#include <utility>

#include "audio_param.h"
#include "cmd_handling.h"
#include "messages.h"
#include "midi_handling.h"
#include "midi_mapping.pb.h"
#include "midi_uart.h"
#include "os/queue.h"
#include "os/timer.h"
#include "persistence.h"
#include "preset_serialisation.h"
#include "presets_manager.h"
#include "selected_preset_manager.h"

namespace shrapnel {

namespace {
constexpr const char *TAG = "main_thread";
}

constexpr const size_t MAX_PARAMETERS = 20;

class ParameterUpdateNotifier;

using AudioParameters = parameters::AudioParameters<MAX_PARAMETERS, 1>;
using SendMessageCallback = etl::delegate<void(const AppMessage &)>;
using MidiMappingType = midi::MappingManager<10, 1>;

class ParameterUpdateNotifier
{
public:
    ParameterUpdateNotifier(std::shared_ptr<AudioParameters> a_audio_params,
                            SendMessageCallback a_send_message)
        : audio_params{std::move(a_audio_params)},
          send_message{a_send_message}
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
    explicit ParameterObserver(
        std::shared_ptr<persistence::Storage> a_persistence)
        : is_save_throttled{true},
          persistence{std::move(a_persistence)},
          timer{"param save throttle",
                pdMS_TO_TICKS(10'000),
                false,
                etl::delegate<void(void)>::create<
                    ParameterObserver<MAX_PARAMETERS>,
                    &ParameterObserver<MAX_PARAMETERS>::timer_callback>(*this)}
    {
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
            if(pdPASS != timer.start(pdMS_TO_TICKS(5)))
            {
                ESP_LOGE(TAG, "Failed to start parameter observer timer");
            }
        }
    }

    void loop()
    {
        if(!is_save_throttled.test_and_set())
        {
            persist_parameters();
            ESP_LOGI(TAG, "Parameters saved to NVS");
        }
    }

private:
    void timer_callback()
    {
        is_save_throttled.clear();
        is_save_throttled.notify_all();
    }

    void persist_parameters()
    {
        for(const auto &param : updated_parameters)
        {
            persistence->save(param.first.data(), param.second);
        }

        updated_parameters.clear();
    }

    std::atomic_flag is_save_throttled;
    std::shared_ptr<persistence::Storage> persistence;
    os::Timer timer;
    etl::map<parameters::id_t, float, MAX_PARAMETERS> updated_parameters;
};

template <typename AudioParametersT>
class PresetLoader
{
public:
    PresetLoader(std::shared_ptr<AudioParametersT> a_parameters,
                 std::shared_ptr<presets::PresetsManager> a_presets_manager,
                 std::shared_ptr<selected_preset::SelectedPresetManager>
                     a_selected_preset_manager,
                 SendMessageCallback a_send_message)
        : parameters{std::move(a_parameters)},
          presets_manager{std::move(a_presets_manager)},
          selected_preset_manager{std::move(a_selected_preset_manager)},
          send_message{a_send_message}
    {
    }

    int load_preset(presets::id_t id)
    {
        int rc = selected_preset_manager->set(id);
        if(rc != 0)
        {
            ESP_LOGE(TAG, "Failed to set selected preset ID");
            return -1;
        }

        presets::PresetData preset{};
        rc = presets_manager->read(id, preset);
        if(rc != 0)
        {
            ESP_LOGE(TAG, "Failed to read preset data");
            return -1;
        }

        presets::deserialise_live_parameters(*parameters, preset.parameters);

        send_message({selected_preset::Notify{
                          .selectedPresetId = id,
                      },
                      std::nullopt});
        return 0;
    }

private:
    std::shared_ptr<AudioParametersT> parameters;
    std::shared_ptr<presets::PresetsManager> presets_manager;
    std::shared_ptr<selected_preset::SelectedPresetManager>
        selected_preset_manager;
    SendMessageCallback send_message;
};

template <std::size_t MAX_PARAMETERS, std::size_t QUEUE_LEN>
class MainThread
{
public:
    MainThread(SendMessageCallback a_send_message,
               Queue<AppMessage, QUEUE_LEN> &a_in_queue,
               midi::MidiUartBase *a_midi_uart,
               std::shared_ptr<AudioParameters> a_audio_params,
               std::shared_ptr<persistence::Storage> a_persistence,
               std::unique_ptr<persistence::Crud<std::span<uint8_t>>>
                   a_midi_mapping_storage,
               std::unique_ptr<persistence::Crud<std::span<uint8_t>>>
                   a_presets_storage)
        : send_message{a_send_message},
          in_queue{a_in_queue},
          parameter_observer{a_persistence},
          clipping_throttle_timer{
              "clipping throttle", pdMS_TO_TICKS(1000), false},
          midi_message_notify_timer{
              "midi notify",
              pdMS_TO_TICKS(100),
              true,
              os::Timer::Callback::
                  create<MainThread, &MainThread::clear_midi_notify_waiting>(
                      *this)},
          midi_uart{a_midi_uart},
          last_midi_message{},
          last_notified_midi_message{},
          midi_decoder{std::make_unique<midi::Decoder>(
              midi::Decoder::Callback::create<MainThread,
                                              &MainThread::on_midi_message>(
                  *this))},
          midi_mutex{},
          audio_params{a_audio_params},
          cmd_handling{
              std::make_unique<parameters::CommandHandling<AudioParameters>>(
                  a_audio_params,
                  parameters::CommandHandling<AudioParameters>::
                      SendMessageCallback::create<
                          MainThread,
                          &MainThread::cmd_handling_send_message>(*this))},
          presets_manager{std::make_shared<presets::PresetsManager>(
              std::move(a_presets_storage))},
          selected_preset_manager{
              std::make_shared<selected_preset::SelectedPresetManager>(
                  a_persistence)}
    {
        auto create_and_load_parameter = [&](const parameters::id_t &name,
                                             float minimum,
                                             float maximum,
                                             float default_value)
        {
            std::optional<float> loaded_value;
            float value;
            int rc = a_persistence->load(name.data(), value);
            if(rc != 0)
            {
                ESP_LOGW(TAG, "Parameter %s failed to load", name.data());
                goto out;
            }

            loaded_value = value;
        out:
            auto range = maximum - minimum;
            rc = a_audio_params->create_and_add_parameter(
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

        a_audio_params->add_observer(parameter_observer);

        parameter_notifier = std::make_shared<ParameterUpdateNotifier>(
            a_audio_params, a_send_message);

        midi_mapping_manager = std::make_shared<MidiMappingType>(
            std::move(a_midi_mapping_storage));

        preset_loader = std::make_shared<PresetLoader<ParameterUpdateNotifier>>(
            parameter_notifier,
            presets_manager,
            selected_preset_manager,
            send_message);

        midi_message_handler = std::make_shared<
            MidiMessageHandler<ParameterUpdateNotifier,
                               MidiMappingType,
                               PresetLoader<ParameterUpdateNotifier>>>(
            parameter_notifier, midi_mapping_manager, preset_loader);

        BaseType_t rc = midi_message_notify_timer.start(portMAX_DELAY);
        if(rc != pdPASS)
        {
            ESP_LOGE(TAG, "Failed to start midi message timer");
        }

        events::input_clipped.test_and_set();
        events::output_clipped.test_and_set();
    }

    void loop()
    {
        {
            auto byte = midi_uart->get_byte(0);
            while(byte.has_value())
            {
                ESP_LOGI(TAG, "midi got byte 0x%02x", *byte);
                midi_decoder->decode(*byte);
                byte = midi_uart->get_byte(0);
            }
        }

        if(AppMessage message; in_queue.receive(&message, 0))
        {
            auto fd = message.second;

            std::visit([this, fd](const auto &m) { handle_message(m, fd); },
                       message.first);
        }

        if(!clipping_throttle_timer.is_active())
        {
            if(!events::input_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "input was clipped");
                send_message({events::InputClipped{}, std::nullopt});
                BaseType_t rc =
                    clipping_throttle_timer.start(pdMS_TO_TICKS(10));
                if(rc != pdPASS)
                {
                    ESP_LOGE(TAG, "Failed to start clipping throttle timer");
                }
            }

            if(!events::output_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "output was clipped");
                send_message({events::OutputClipped{}, std::nullopt});
                BaseType_t rc =
                    clipping_throttle_timer.start(pdMS_TO_TICKS(10));
                if(rc != pdPASS)
                {
                    ESP_LOGE(TAG, "Failed to start clipping throttle timer");
                }
            }
        }

        parameter_observer.loop();

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
    void handle_message(const auto &app_message, std::optional<int> fd)
    {
        using AppMessageT = std::decay_t<decltype(app_message)>;

        if constexpr(std::is_same_v<AppMessageT, parameters::ApiMessage>)
        {
            if(!fd.has_value())
            {
                ESP_LOGE(TAG, "Must always have fd");
            }

            cmd_handling->dispatch(app_message, *fd);
        }
        else if constexpr(std::is_same_v<AppMessageT, midi::MappingApiMessage>)
        {
            std::scoped_lock lock{midi_mutex};

            auto response = std::visit(
                [&](const auto &midi_mapping_message)
                    -> std::optional<midi::MappingApiMessage>
                {
                    using MidiMappingMessageT =
                        std::decay_t<decltype(midi_mapping_message)>;

                    if constexpr(std::is_same_v<MidiMappingMessageT,
                                                midi::GetRequest>)
                    {
                        auto mappings = midi_mapping_manager->get();
                        for(const auto &[id, mapping] : *mappings)
                        {
                            send_message({
                                midi::Update{
                                    {
                                        id,
                                        mapping,
                                    },
                                },
                                std::nullopt,
                            });
                        }
                    }
                    else if constexpr(std::is_same_v<MidiMappingMessageT,
                                                     midi::CreateRequest>)
                    {
                        uint32_t id;
                        auto rc = midi_mapping_manager->create(
                            midi_mapping_message.mapping, id);
                        if(rc == 0)
                        {
                            return midi::CreateResponse{
                                .mapping{
                                    id,
                                    midi_mapping_message.mapping,
                                },
                            };
                        }
                    }
                    else if constexpr(std::is_same_v<MidiMappingMessageT,
                                                     midi::Update>)
                    {
                        // return code ignored, as there is no way
                        // to indicate the error to the frontend
                        (void)midi_mapping_manager->update(
                            midi_mapping_message.mapping.first,
                            midi_mapping_message.mapping.second);
                    }
                    else if constexpr(std::is_same_v<MidiMappingMessageT,
                                                     midi::Remove>)
                    {
                        // ignored because no way to report error to frontend
                        (void)midi_mapping_manager->destroy(
                            midi_mapping_message.id);
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Unhandled message type");
                    }

                    return std::nullopt;
                },
                app_message);

            if(response.has_value())
            {
                send_message({*response, std::nullopt});
            }
        }
        else if constexpr(std::is_same_v<AppMessageT,
                                         presets::PresetsApiMessage>)
        {
            auto response = std::visit(
                [&](const auto &presets_message)
                    -> std::optional<presets::PresetsApiMessage>
                {
                    using PresetsMessageT =
                        std::decay_t<decltype(presets_message)>;

                    if constexpr(std::is_same_v<PresetsMessageT,
                                                presets::Initialise>)
                    {
                        presets_manager->for_each(

                            [this](presets::id_t id,
                                   const presets::PresetData &preset)
                            {
                                send_message({presets::Notify{
                                                  .id = id,
                                                  .preset = preset,
                                              },
                                              std::nullopt});
                            });
                    }
                    else if constexpr(std::is_same_v<PresetsMessageT,
                                                     presets::Create>)
                    {
                        presets::id_t id;
                        int rc =
                            presets_manager->create(presets_message.preset, id);
                        if(rc != 0)
                        {
                            ESP_LOGE(TAG, "Failed to create preset");
                            return std::nullopt;
                        }
                        return presets::Notify{
                            .id{id},
                            .preset{presets_message.preset},
                        };
                    }
                    else if constexpr(std::is_same_v<PresetsMessageT,
                                                     presets::Update>)
                    {
                        int rc = presets_manager->update(
                            presets_message.id, presets_message.preset);
                        if(rc != 0)
                        {
                            ESP_LOGE(TAG, "Failed to update preset");
                            return std::nullopt;
                        }

                        return presets::Notify{
                            .id{presets_message.id},
                            .preset{presets_message.preset},
                        };
                    }
                    else if constexpr(std::is_same_v<PresetsMessageT,
                                                     presets::Delete>)
                    {
                        int rc = presets_manager->destroy(presets_message.id);
                        if(rc != 0)
                        {
                            ESP_LOGE(TAG, "Failed to destroy preset");
                            return std::nullopt;
                        }
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Unhandled message type");
                    }

                    return std::nullopt;
                },
                app_message);

            if(response.has_value())
            {
                send_message({*response, std::nullopt});
            }
        }
        else if constexpr(std::is_same_v<
                              AppMessageT,
                              selected_preset::SelectedPresetApiMessage>)
        {
            auto response = std::visit(
                [this](const auto &message)
                    -> std::optional<selected_preset::SelectedPresetApiMessage>
                { return handle_selected_preset_message(message); },
                app_message);

            if(response.has_value())
            {
                send_message({*response, std::nullopt});
            }
        }
    }

    std::optional<selected_preset::SelectedPresetApiMessage>
    handle_selected_preset_message(selected_preset::Read)
    {
        presets::id_t id;
        int rc = selected_preset_manager->get(id);
        if(rc != 0)
        {
            return std::nullopt;
        }

        return selected_preset::Notify{.selectedPresetId = id};
    }

    std::optional<selected_preset::SelectedPresetApiMessage>
    handle_selected_preset_message(selected_preset::Notify)
    {
        ESP_LOGW(TAG,
                 "Ignored selected preset notify message, the frontend is"
                 "not supposed to send this");
        return std::nullopt;
    }

    std::optional<selected_preset::SelectedPresetApiMessage>
    handle_selected_preset_message(selected_preset::Write write)
    {
        presets::id_t id = write.selectedPresetId;
        preset_loader->load_preset(id);
        return std::nullopt;
    }

    void on_midi_message(midi::Message message)
    {
        etl::string<40> buffer;
        etl::string_stream stream{buffer};
        stream << message;
        ESP_LOGI(TAG, "%s", buffer.data());

        last_midi_message = message;

        {
            std::scoped_lock lock{midi_mutex};
            midi_message_handler->process_message(message);
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
    std::shared_ptr<MidiMappingType> midi_mapping_manager;
    std::shared_ptr<MidiMessageHandler<ParameterUpdateNotifier,
                                       MidiMappingType,
                                       PresetLoader<ParameterUpdateNotifier>>>
        midi_message_handler;
    std::shared_ptr<AudioParameters> audio_params;
    std::unique_ptr<parameters::CommandHandling<
        parameters::AudioParameters<MAX_PARAMETERS, 1>>>
        cmd_handling;
    std::shared_ptr<presets::PresetsManager> presets_manager;
    std::shared_ptr<selected_preset::SelectedPresetManager>
        selected_preset_manager;
    std::shared_ptr<ParameterUpdateNotifier> parameter_notifier;
    std::shared_ptr<PresetLoader<ParameterUpdateNotifier>> preset_loader;
};

} // namespace shrapnel