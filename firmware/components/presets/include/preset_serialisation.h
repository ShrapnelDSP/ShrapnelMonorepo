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

#include "presets.pb.h"
#include "presets_api.h"
#include <esp_log.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <span>

namespace shrapnel::presets {

template <typename T>
ParametersData serialise_live_parameters(T &parameters_service)
{
    shrapnel::presets::ParametersData parameters{
        .amp_gain = parameters_service.get("ampGain"),
        .amp_channel = parameters_service.get("ampChannel"),
        .bass = parameters_service.get("bass"),
        .middle = parameters_service.get("middle"),
        .treble = parameters_service.get("treble"),
        .contour = parameters_service.get("contour"),
        .volume = parameters_service.get("volume"),
        .noise_gate_threshold = parameters_service.get("noiseGateThreshold"),
        .noise_gate_hysteresis = parameters_service.get("noiseGateHysteresis"),
        .noise_gate_attack = parameters_service.get("noiseGateAttack"),
        .noise_gate_hold = parameters_service.get("noiseGateHold"),
        .noise_gate_release = parameters_service.get("noiseGateRelease"),
        .noise_gate_bypass = parameters_service.get("noiseGateBypass"),
        .chorus_rate = parameters_service.get("chorusRate"),
        .chorus_depth = parameters_service.get("chorusDepth"),
        .chorus_mix = parameters_service.get("chorusMix"),
        .chorus_bypass = parameters_service.get("chorusBypass"),
        .wah_position = parameters_service.get("wahPosition"),
        .wah_vocal = parameters_service.get("wahVocal"),
        .wah_bypass = parameters_service.get("wahBypass"),
    };

    return parameters;
}

template <typename T>
void deserialise_live_parameters(T &parameters_service,
                                 shrapnel::presets::ParametersData parameters)
{
    parameters_service.update("ampGain", parameters.amp_gain);
    parameters_service.update("ampChannel", parameters.amp_channel);
    parameters_service.update("bass", parameters.bass);
    parameters_service.update("middle", parameters.middle);
    parameters_service.update("treble", parameters.treble);
    parameters_service.update("contour", parameters.contour);
    parameters_service.update("volume", parameters.volume);
    parameters_service.update("noiseGateThreshold",
                              parameters.noise_gate_threshold);
    parameters_service.update("noiseGateHysteresis",
                              parameters.noise_gate_hysteresis);
    parameters_service.update("noiseGateAttack", parameters.noise_gate_attack);
    parameters_service.update("noiseGateHold", parameters.noise_gate_hold);
    parameters_service.update("noiseGateRelease",
                              parameters.noise_gate_release);
    parameters_service.update("noiseGateBypass", parameters.noise_gate_bypass);
    parameters_service.update("chorusRate", parameters.chorus_rate);
    parameters_service.update("chorusDepth", parameters.chorus_depth);
    parameters_service.update("chorusMix", parameters.chorus_mix);
    parameters_service.update("chorusBypass", parameters.chorus_bypass);
    parameters_service.update("wahPosition", parameters.wah_position);
    parameters_service.update("wahVocal", parameters.wah_vocal);
    parameters_service.update("wahBypass", parameters.wah_bypass);
}

} // namespace shrapnel::presets