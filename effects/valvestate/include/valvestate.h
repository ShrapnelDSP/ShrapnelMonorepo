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
 *
 */

#pragma once

#include <cstddef>
#include "input_filter.h"
#include "gain_control.h"
#include "clipping.h"
#include "fmv.h"
#include "contour.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

class Valvestate
{
    public:
    Valvestate();

    void set_gain(float gain, float channel);

    void set_fmv(float bass, float middle, float treble);

    void set_contour(float contour);

    void set_volume(float volume);

    void process(float *buffer, std::size_t buffer_size);

    void prepare(float samplerate);
    void reset(void);

    private:
    InputFilter input;
    GainControl gaincontrol;
    Clipping clipping;
    FMVFilter fmv;
    ContourFilter contour;
    float volume;
};

}
}
}
