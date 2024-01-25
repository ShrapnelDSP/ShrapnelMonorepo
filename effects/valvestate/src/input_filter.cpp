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

#include "input_filter.h"
#include <cmath>

namespace shrapnel {
namespace effect {
namespace valvestate {

void InputFilter::prepare(float samplerate, size_t)
{
    float K = 2 * samplerate;
    float B0 = 1.10449884469421e-9f * std::pow(K, 2.f) +
               0.00246545127613797f * K + 0.5f;
    float B1 = 1.0f - 2.20899768938842e-9f * std::pow(K, 2.f);
    float B2 = 1.10449884469421e-9f * std::pow(K, 2.f) -
               0.00246545127613797f * K + 0.5f;

    float A0 = 1.10449884469421e-9f * std::pow(K, 2.f) +
               0.000115449950739352f * K + 0.5f;
    float A1 = 1.0f - 2.20899768938842e-9f * std::pow(K, 2.f);
    float A2 = 1.10449884469421e-9f * std::pow(K, 2.f) -
               0.000115449950739352f * K + 0.5f;

    filter.set_coefficients(std::array<float, 6>({B0, B1, B2, A0, A1, A2}));

    filter.reset();
}

void InputFilter::process(std::span<float> buffer)
{
    filter.process(buffer.data(), buffer.data(), buffer.size());
}

void InputFilter::reset() { filter.reset(); }

} // namespace valvestate
} // namespace effect
} // namespace shrapnel
