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

#include "iir_concrete.h"
#include <cstddef>

namespace shrapnel::effect {

class Wah
{
public:
    Wah();

    /** Set the expression pedal position
     *
     * This parameter is normalised to the range [0, 1]. 1 corresponds to the
     * pedal being fully depressed.
     */
    void set_expression_position(float position);

    /** Set the vocal mod magnitude
     *
     * This parameter is normalised to the range [0, 1]. Larger values increase
     * the resonance filter bandwidth.
     */
    void set_vocal(float vocal);

    void set_sample_rate(float rate);

    void process(float *samples, std::size_t sample_count);

private:
    float sample_rate = 0;

    float position_parameter;
    float vocal_parameter;

    shrapnel::dsp::IirFilter wah_filter;
    shrapnel::dsp::IirFilter position_filter;
    shrapnel::dsp::IirFilter vocal_filter;
};

}