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

#pragma once

#include <array>
#include <cstddef>

namespace shrapnel {
namespace dsp {

class IirFilterBase {
    public:
    virtual void process(const float *in, float *out, std::size_t buf_size) = 0;

    virtual void reset(void) = 0;

    virtual void set_coefficients(std::array<float, 6> new_coefficients) = 0;
    virtual void set_coefficients(std::array<float, 8> new_coefficients) = 0;
    virtual void set_coefficients(std::array<float, 10> new_coefficients) = 0;
};

}
}
