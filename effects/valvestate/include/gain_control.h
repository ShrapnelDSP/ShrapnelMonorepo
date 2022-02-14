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

#pragma once

#include "iir_concrete.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

class GainControl
{
    public:
        /**
         * \param g The value of gain from 0 to 1.
         * \param channel 0 for OD1, 1 for OD2
         */
        void set_parameters(float g, float channel);

        void prepare(float samplerate);
        void process(float *buffer, std::size_t buffer_size);
        void reset(void);

    private:
        float samplerate;

        shrapnel::dsp::IirFilter filter;
};

}
}
}
