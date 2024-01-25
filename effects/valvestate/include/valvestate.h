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

#include "clipping.h"
#include "contour.h"
#include "dsp_concepts.h"
#include "fmv.h"
#include "gain_control.h"
#include "input_filter.h"
#include <cstddef>

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

    void prepare(float samplerate, size_t);
    void process(std::span<float> buffer);
    void reset();

private:
    InputFilter input;
    GainControl gaincontrol;
    Clipping clipping;
    FMVFilter fmv;
    ContourFilter contour;
    float volume;
};
static_assert(dsp::Processor<Valvestate>);

} // namespace valvestate
} // namespace effect
} // namespace shrapnel
