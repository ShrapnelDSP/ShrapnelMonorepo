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

#include "chorus.h"
#include <cmath>

#define MAX_DELAY_MS 15.f

namespace {

float triangle(float phase)
{
    if(phase < (float)M_PI)
    {
        return 1 - 2 * phase / (float)M_PI;
    }
    else
    {
        return - 3 + 2 * phase / (float)M_PI;
    }
}

}


namespace shrapnel {
namespace effect {

Chorus::Chorus() {
}

void Chorus::set_modulation_rate_hz(float rate)
{
    modulation_rate = rate;
}

void Chorus::set_modulation_depth(float depth)
{
    modulation_depth = depth;
}

void Chorus::set_modulation_mix(float mix)
{
    modulation_mix = mix;
}

void Chorus::process(float *samples, std::size_t sample_count)
{
    for(std::size_t i = 0; i < sample_count; i++)
    {
        float lfo = 0.5f * triangle(phase);
        phase += modulation_rate / sample_rate * 2 * (float)M_PI;

        if(phase > 2 * (float)M_PI)
        {
            phase -= 2 * (float)M_PI;
        }

#if 1
        float delay = MAX_DELAY_MS / 1000 * sample_rate *
                (0.5f + (modulation_depth * lfo));

        dspal_delayline_set_delay(delayline, delay);

        dspal_delayline_push_sample(delayline, samples[i]);
        samples[i] = samples[i] +
            (modulation_mix * dspal_delayline_pop_sample(delayline));
#else
        samples[i] = lfo;
#endif
    }
}


void Chorus::set_sample_rate(float rate)
{
    if(nullptr != delayline)
    {
        dspal_delayline_destroy(delayline);
        delayline = nullptr;
    }

    delayline = dspal_delayline_create(rate * MAX_DELAY_MS / 1000);

    sample_rate = rate;
};

}
}
