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

namespace shrapnel::effect {

void Chorus::set_modulation_rate_hz(float rate)
{
    lfo_increment = rate / sample_rate * -2;
}

void Chorus::set_modulation_depth(float depth) { modulation_depth = depth; }

void Chorus::set_modulation_mix(float mix) { modulation_mix = mix; }

void Chorus::process(std::span<float> a_samples)
{
    float *samples = a_samples.data();

    // WTF, gcc is generating complete nonsense like:
    // lfo_tmp += lfo_increment_tmp;
    //
    // lfo_tmp += lfo_increment_tmp;
    // 4008b188:       fa1f00          mov.s   f1, f15
    // 4008b18b:       fa0e00          mov.s   f0, f14
    // 4008b18e:       0a0100          add.s   f0, f1, f0
    // 4008b191:       faf000          mov.s   f15, f0
    register float lfo_increment_tmp asm("f14") = lfo_increment;
    register float lfo_tmp asm("f15") = lfo;
    for(int i = 0; i < a_samples.size(); i++)
    {
        lfo_tmp += lfo_increment_tmp;

        if(lfo_tmp > 0.5f)
        {
            lfo_tmp = 0.5f;
            lfo_increment_tmp = -lfo_increment_tmp;
        }

        if(lfo_tmp < -0.5f)
        {
            lfo_tmp = -0.5f;
            lfo_increment_tmp = -lfo_increment_tmp;
        }

#if 0
        float delay = MAX_DELAY_MS / 1000 * sample_rate *
                      (0.5f + (modulation_depth * lfo));

        delayline->set_delay(delay);
        delayline->push_sample(samples[i]);
        samples[i] = samples[i] + (modulation_mix * delayline->pop_sample());
#else
        samples[i] = lfo_tmp;
#endif
    }

    lfo = lfo_tmp;
    lfo_increment = lfo_increment_tmp;
}

void Chorus::prepare(float rate, size_t)
{
    reset();
    delayline = std::make_unique<dsp::DelayLine>(rate * MAX_DELAY_MS / 1000);
    sample_rate = rate;
}

void Chorus::reset() { delayline = nullptr; }

} // namespace shrapnel::effect
