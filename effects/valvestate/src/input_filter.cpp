/*
    Copyright 2020 Barabas Raffai

    This file is part of Valvestate Sim.

    Valvestate Sim is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    Valvestate Sim is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License along
    with Valvestate Sim.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "input_filter.h"
#include <cmath>

namespace shrapnel {
namespace effect {
namespace valvestate {

void InputFilter::prepare(float samplerate)
{
    float K = 2 * samplerate;
    float B0 = 1.10449884469421e-9f*std::pow(K, 2) + 0.00246545127613797f*K + 0.5f;
    float B1 = 1.0 - 2.20899768938842e-9f*std::pow(K, 2);
    float B2 = 1.10449884469421e-9f*std::pow(K, 2) - 0.00246545127613797f*K + 0.5f;

    float A0 = 1.10449884469421e-9f*std::pow(K, 2) + 0.000115449950739352f*K + 0.5f;
    float A1 = 1.0 - 2.20899768938842e-9f*std::pow(K, 2);
    float A2 = 1.10449884469421e-9f*std::pow(K, 2) - 0.000115449950739352f*K + 0.5f;

    filter.set_coefficients(std::array<float, 6>({B0, B1, B2, A0, A1, A2}));

    filter.reset();
}

void InputFilter::process(float *buffer, std::size_t buffer_size)
{
    filter.process(buffer, buffer, buffer_size);
}

void InputFilter::reset()
{
    filter.reset();
}

}
}
}
