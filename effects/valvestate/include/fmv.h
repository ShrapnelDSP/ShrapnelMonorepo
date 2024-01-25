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

namespace shrapnel {
namespace effect {
namespace valvestate {

class FMVFilter
{
public:
    void set_parameters(float l, float m, float t);

    void prepare(float samplerate, size_t);
    void process(std::span<float> buffer);
    void reset();

private:
    shrapnel::dsp::IirFilter filter;
    float samplerate;
};

static_assert(dsp::Processor<FMVFilter>);

} // namespace valvestate
} // namespace effect
} // namespace shrapnel
