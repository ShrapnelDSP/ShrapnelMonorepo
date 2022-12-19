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

#include <cmath>
#include "dsps_biquad_gen.h"
#include "dsps_mulc.h"
#include <numbers>

namespace shrapnel::effect {

Wah::Wah() {}

void Wah::set_expression_position(float a_position) { position = a_position; }

void Wah::set_sample_rate(float rate) {
    float p = 0.9;
    position_filter.set_coefficients(
        std::array<float, 6>{1 - p, 0, 0, 1, -p, 0});
    sample_rate = rate;
}

void Wah::process(float *samples, std::size_t sample_count)
{
    constexpr auto vocal = 0;

    float wah;
    position_filter.process(&position, &wah, 1);

    constexpr auto base_gain = 14.f;
    constexpr auto gain_factor_wah = -1.2f;
    constexpr auto gain_factor_vocal = -1.f;

    auto gain =
        base_gain * expf(gain_factor_wah * wah + gain_factor_vocal * vocal);
    auto resonance_frequency_hz = 450.f * powf(2, 2.3f * wah);
    auto quality = powf(2, 2 * (1 - wah) + 1);

    float coefficients[5];

    // XXX: Slight inefficiency here. This function converts from denormalised
    // to normalised, then set_coefficients does the same again
    dsps_biquad_gen_bpf0db_f32(
        coefficients, resonance_frequency_hz / sample_rate, quality);

    wah_filter.set_coefficients(
        std::array<float, 6>{
            coefficients[0] * gain,
            coefficients[1] * gain,
            coefficients[2] * gain,
            1,
            coefficients[3],
            coefficients[4]});

    wah_filter.process(samples, samples, sample_count);
}

}
