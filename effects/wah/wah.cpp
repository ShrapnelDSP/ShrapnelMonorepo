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

#include "wah.h"

#include "dsps_biquad_gen.h"
#include "dsps_mulc.h"
#include <cmath>
#include <numbers>

namespace shrapnel::effect {

void Wah::set_expression_position(float position)
{
    position_parameter = position;
}

void Wah::set_vocal(float vocal) { vocal_parameter = vocal; }

void Wah::prepare(float rate, size_t)
{
    float p = 0.9;
    std::array<float, 6> coefficients{1 - p, 0, 0, 1, -p, 0};
    position_filter.set_coefficients(coefficients);
    vocal_filter.set_coefficients(coefficients);
    sample_rate = rate;
}

void Wah::process(std::span<float> samples)
{
    float wah;
    position_filter.process(&position_parameter, &wah, 1);

    float vocal;
    vocal_filter.process(&vocal_parameter, &vocal, 1);

    constexpr auto base_gain = 14.f * std::pow(10.f, -1.f / 20.f);
    constexpr auto gain_factor_wah = -1.2f;
    constexpr auto gain_factor_vocal = -1.f;
    constexpr auto vocal_range = 2.f;

    auto gain =
        base_gain * expf(gain_factor_wah * wah + gain_factor_vocal * vocal);
    auto resonance_frequency_hz = 450.f * powf(2, 2.3f * wah);
    auto quality = powf(2, 2 * (1 - wah) + 1 - vocal_range * vocal);

    float coefficients[5];

    // XXX: Slight inefficiency here. This function converts from denormalised
    // to normalised, then set_coefficients does the same again
    dsps_biquad_gen_bpf0db_f32(
        coefficients, resonance_frequency_hz / sample_rate, quality);

    wah_filter.set_coefficients(std::array<float, 6>{coefficients[0] * gain,
                                                     coefficients[1] * gain,
                                                     coefficients[2] * gain,
                                                     1,
                                                     coefficients[3],
                                                     coefficients[4]});

    wah_filter.process(samples.data(), samples.data(), samples.size());
}

void Wah::reset()
{
    wah_filter.reset();
    position_filter.reset();
    vocal_filter.reset();
}

} // namespace shrapnel::effect
