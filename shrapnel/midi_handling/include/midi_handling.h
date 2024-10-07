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

#include "etl/delegate.h"
#include "messages.h"
#include "midi_mapping.h"
#include "midi_protocol.h"
#include "presets_manager.h"
#include "selected_preset_manager.h"

namespace shrapnel {

template <typename AudioParametersT,
          typename MappingManagerT,
          typename PresetLoaderT>
class MidiMessageHandler
{
public:
    MidiMessageHandler(std::shared_ptr<AudioParametersT> a_parameters,
                       std::shared_ptr<MappingManagerT> a_mapping_manager,
                       std::shared_ptr<PresetLoaderT> a_preset_loader)
        : parameters{std::move(a_parameters)},
          mapping_manager{std::move(a_mapping_manager)},
          preset_loader{std::move(a_preset_loader)} {};

    /** React to a MIDI message by updating an audio parameter if there is a
     * mapping registered
     */
    void process_message(midi::Message message) const
    {
        auto cc_params =
            get_if<midi::Message::ControlChange>(&message.parameters);
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
            case midi::Mapping::Mode::PARAMETER:
                parameters->update(mapping.parameter_name,
                                   cc_params->value /
                                       float(midi::CC_VALUE_MAX));
                break;
            case midi::Mapping::Mode::TOGGLE:
            {
                if(cc_params->value == 0)
                {
                    continue;
                }

                auto old_value = parameters->get(mapping.parameter_name);

                parameters->update(mapping.parameter_name,
                                   old_value < 0.5f ? 1.f : 0.f);
                break;
            }
            case midi::Mapping::Mode::BUTTON:
            {
                // A button sends the maximum value when it becomes pressed.
                if(cc_params->value != midi::CC_VALUE_MAX)
                {
                    continue;
                }

                auto id = mapping.preset_id;
                preset_loader->load_preset(id);
                break;
            }
            }
        }
    }

private:
    std::shared_ptr<AudioParametersT> parameters;
    std::shared_ptr<MappingManagerT> mapping_manager;
    std::shared_ptr<PresetLoaderT> preset_loader;
};

} // namespace shrapnel