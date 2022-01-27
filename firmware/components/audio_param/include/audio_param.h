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

#include "esp_err.h"

typedef enum {
    PARAM_TIGHT,
    PARAM_HM2_GAIN,
    PARAM_AMP_GAIN,
    PARAM_BASS,
    PARAM_MIDDLE,
    PARAM_TREBLE,
    PARAM_VOLUME,
    PARAM_GATE_THRESHOLD,
    PARAM_MAX,
} audio_param_t;

namespace shrapnel {

class AudioParametersBase {
    public:
        virtual esp_err_t update(audio_param_t param, float value) = 0;
};

class AudioParameters : public AudioParametersBase {
    public:
        esp_err_t update(audio_param_t param, float value) override;
};

};
