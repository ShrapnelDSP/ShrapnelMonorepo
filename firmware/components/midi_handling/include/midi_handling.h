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

#include "midi_mapping.h"
#include "midi_protocol.h"

namespace shrapnel::midi {

template <typename AudioParametersT, typename MappingManagerT>
class MessageHandler
{
public:
    /* TODO: notify about preset changes here
    using SendMessageCallback =
        etl::delegate<void(const ApiMessage &, std::optional<int>)>;
    */
    MessageHandler(std::shared_ptr<AudioParametersT> a_parameters,
                   std::shared_ptr<MappingManagerT> a_mapping_manager,
                   std::shared_ptr<presets::PresetsManager> a_presets_manager,
                   std::shared_ptr<selected_preset::SelectedPresetManager>
                       a_selected_preset_manager)
        : parameters{std::move(a_parameters)},
          mapping_manager{std::move(a_mapping_manager)},
          presets_manager{std::move(a_presets_manager)},
          selected_preset_manager{std::move(a_selected_preset_manager)}
    {
    }

    /** React to a MIDI message by updating an audio parameter if there is a
     * mapping registered
     */
    void process_message(Message message) const
    {
        auto cc_params = get_if<Message::ControlChange>(&message.parameters);
        if(!cc_params)
            return;

        for(const auto &[_, mapping] : *mapping_manager)
        {
            if(mapping.midi_channel != message.channel)
            {
                continue;
            }

            if(mapping.cc_number != cc_params->control)
            {
                continue;
            }

            switch(mapping.mode)
            {
            case Mapping::Mode::PARAMETER:
                parameters->update(*mapping.parameter_name,
                                   cc_params->value / float(CC_VALUE_MAX));
                break;
            case Mapping::Mode::TOGGLE:
            {
                if(cc_params->value == 0)
                {
                    continue;
                }

                auto old_value = parameters->get(*mapping.parameter_name);

                parameters->update(*mapping.parameter_name,
                                   old_value < 0.5f ? 1.f : 0.f);
                break;
            }
            case Mapping::Mode::BUTTON:
            {
                if(cc_params->value != 0x7F)
                {
                    continue;
                }

                // FIXME: this is duplicated from the main_thread.h.
                // - Create some internal app events that are decoupled from the
                //   API.
                // - Make the API write message emit an select preset event
                // - Emit the select preset event here
                //
                // Or maybe:
                //
                // Make the selected preset manager a notifier. Call set on it
                // from here and from main, causing it to notify. When it
                // notifies, then a preset loader reads the preset and updates
                // parameters.
                int rc = selected_preset_manager->set(*mapping.preset_id);
                if(rc != 0)
                {
                    continue;
                }

                auto preset = presets_manager->read(*mapping.preset_id);
                if(!preset.has_value())
                {
                    continue;
                }

                deserialise_live_parameters(*parameters, preset->parameters);
                break;
            }
            }
        }
    };

private:
    std::shared_ptr<AudioParametersT> parameters;
    std::shared_ptr<MappingManagerT> mapping_manager;
    std::shared_ptr<presets::PresetsManager> presets_manager;
    std::shared_ptr<selected_preset::SelectedPresetManager>
        selected_preset_manager;
};

} // namespace shrapnel::midi