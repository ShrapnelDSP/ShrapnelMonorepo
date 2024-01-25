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

#include "dsp_concepts.h"
#include "iir_concrete.h"

namespace shrapnel::effect::valvestate {

class GainControl
{
public:
    /**
         * \param g The value of gain from 0 to 1.
         * \param channel 0 for OD1, 1 for OD2
         */
    void set_parameters(float g, float channel);

    void prepare(float samplerate, size_t);
    void process(std::span<float> buffer);
    void reset();

private:
    float samplerate;

    shrapnel::dsp::IirFilter filter;
};

static_assert(dsp::Processor<GainControl>);

} // namespace shrapnel::effect::valvestate
