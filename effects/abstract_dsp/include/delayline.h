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

#pragma once
#include <cstddef>
#include <vector>

namespace shrapnel {
namespace dsp {

class DelayLine {
    public:
    DelayLine(size_t max_samples);

    void push_sample(float sample);
    float pop_sample(void);

    void set_delay(float new_delay);

    private:
    std::vector<float> samples;

    size_t writeIndex;

    float delay;
};

}
}
