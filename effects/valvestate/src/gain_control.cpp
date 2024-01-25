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

#include "gain_control.h"
#include <cmath>

namespace shrapnel::effect::valvestate {

void GainControl::set_parameters(float g, float channel)
{
    float B0, B1, B2, A0, A1, A2;
    float K = 2 * samplerate;

    if(channel < 0.5f)
    {
        //OD1
        B0 = 0.00916582638188652f * K * g;
        B1 = 0;
        B2 = -0.00916582638188652f * K * g;

        A0 = 4.30793839948666e-8f * std::pow(K, 2.f) * g + 2.35e-5f * K * g +
             0.000916582638188652f * K + 0.5f;
        A1 = -8.61587679897332e-8f * std::pow(K, 2.f) * g + 1.0f;
        A2 = 4.30793839948666e-8f * std::pow(K, 2.f) * g - 2.35e-5f * K * g -
             0.000916582638188652f * K + 0.5f;
    }
    else
    {
        //OD2
        B0 = 0.11f * K * g;
        B1 = 0;
        B2 = -0.11f * K * g;

        A0 = 4.69953e-8f * std::pow(K, 2.f) * g + 2.35e-5f * K * g +
             0.0009999f * K + 0.5f;
        A1 = -9.39906e-8f * std::pow(K, 2.f) * g + 1.0f;
        A2 = 4.69953e-8f * std::pow(K, 2.f) * g - 2.35e-5f * K * g -
             0.0009999f * K + 0.5f;
    }

    filter.set_coefficients(std::array<float, 6>({B0, B1, B2, A0, A1, A2}));
}

void GainControl::prepare(float a_samplerate, size_t)
{
    filter.reset();
    samplerate = a_samplerate;
}

void GainControl::process(std::span<float> buffer)
{
    filter.process(buffer.data(), buffer.data(), buffer.size());
}

void GainControl::reset() { filter.reset(); }

} // namespace shrapnel::effect::valvestate
