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
#include <numbers>

namespace shrapnel::effect {

Wah::Wah() {}

void Wah::set_expression_position(float a_position) { position = a_position; }

void Wah::set_sample_rate(float rate) {
    float p = 0.9;
    parameter_filter.set_coefficients(
        std::array<float, 6>{1 - p, 0, 0, 1, -p, 0});
    sample_rate = rate;
}

void Wah::process(float *samples, std::size_t sample_count)
{
    // From DAFX book section 12.2.4 Wah-wah filter
    float current_position;
    parameter_filter.process(&position, &current_position, 1);

    // power tweaked for more even loudness across the sweep
    auto gain = 0.1f * powf(4, 2 * current_position);
    auto resonance_frequency_hz = 450.f * powf(2, 2.3f * current_position);
    auto quality = powf(2, 2 * (1 - current_position) + 1);

    auto frequency_ratio = resonance_frequency_hz / sample_rate;
    auto pole_radius =
        1 - std::numbers::pi_v<float> * frequency_ratio / quality;
    auto pole_angle = 2 * std::numbers::pi_v<float> * frequency_ratio;

    auto a1 = -2.f * pole_radius * std::cos(pole_angle);
    auto a2 = pole_radius * pole_radius;

    wah_filter.set_coefficients(
        std::array<float, 6>{gain, -gain, 0, 1, a1, a2});

    wah_filter.process(samples, samples, sample_count);
}

}
