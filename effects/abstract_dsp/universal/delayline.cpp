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

#include "delayline.h"
#include <cmath>

namespace shrapnel {
namespace dsp {

DelayLine::DelayLine(size_t max_samples) :
    samples(max_samples + 1),
    writeIndex(0) { }

void DelayLine::push_sample(float sample)
{
    samples[writeIndex] = sample;

    writeIndex++;
    writeIndex %= samples.size();
}

float DelayLine::pop_sample(void)
{
    auto length = samples.size();

    auto integral_delay = static_cast<size_t>(std::floor(delay));
    float fractional_delay = delay - integral_delay;

    auto last_written_index = (writeIndex - 1 + length) % length;
    auto before_last_written_index = (last_written_index - 1 + length) % length;

    auto sample = samples[(last_written_index - integral_delay + length) % length];
    auto before_sample = samples[(before_last_written_index - integral_delay + length) % length];

    return (1 - fractional_delay) * sample + fractional_delay * before_sample;
}

void DelayLine::set_delay(float new_delay)
{
    delay = std::min((float)samples.size() - 1, std::max(0.f, new_delay));
}

}
}
