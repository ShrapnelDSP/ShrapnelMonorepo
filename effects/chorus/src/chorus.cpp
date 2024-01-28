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
        return -3 + 2 * phase / (float)M_PI;
    }
}

} // namespace

namespace shrapnel::effect {

void Chorus::set_modulation_rate_hz(float rate) { modulation_rate = rate; }

void Chorus::set_modulation_depth(float depth) { modulation_depth = depth; }

void Chorus::set_modulation_mix(float mix) { modulation_mix = mix; }

void Chorus::process(std::span<float> a_samples)
{
    float *samples = a_samples.data();
    for(int i = 0; i < a_samples.size(); i++)
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

        delayline->set_delay(delay);
        delayline->push_sample(samples[i]);
        samples[i] = samples[i] + (modulation_mix * delayline->pop_sample());
#else
        samples[i] = lfo;
#endif
    }
}

void Chorus::prepare(float rate, size_t)
{
    reset();
    delayline = std::make_unique<dsp::DelayLine>(rate * MAX_DELAY_MS / 1000);
    sample_rate = rate;
}

void Chorus::reset() { delayline = nullptr; }

} // namespace shrapnel::effect
