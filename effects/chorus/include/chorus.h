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

#include "abstract_dsp.h"
#include "dsp_concepts.h"
#include "cstddef"

namespace shrapnel::effect {

class Chorus {
    public:
    /** Set the rate of modulation
     *
     * This parameter is in units of Hz.
     */
    void set_modulation_rate_hz(float rate);

    /** Set the depth of modulation
     *
     * This parameter is normalised to the range [0, 1].
     */
    void set_modulation_depth(float depth);

    /** Set the mix ratio of modulation
     *
     * This parameter is normalised to the range [0, 1].
     */
    void set_modulation_mix(float depth);

    void prepare(float, size_t);
    void process(std::span<float> samples);
    void reset();

    private:
    float modulation_rate = 0;
    float modulation_depth = 0;
    float modulation_mix = 0;

    float sample_rate = 0;

    float phase = 0;

    dspal_delayline_t delayline = nullptr;
};

static_assert(dsp::Processor<Chorus>);

}
