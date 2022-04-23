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

#include "valvestate.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

Valvestate::Valvestate() : volume(0) {}

void Valvestate::set_gain(float gain, float channel)
{
    gaincontrol.set_parameters(gain, channel);
}

void Valvestate::set_fmv(float bass, float middle, float treble)
{
    fmv.set_parameters(bass, middle, treble);
}

void Valvestate::set_contour(float a_contour)
{
    contour.set_parameter(a_contour);
}

void Valvestate::set_volume(float a_volume)
{
    volume = a_volume;
}

void Valvestate::process(float *buffer, std::size_t buffer_size)
{
    input.process(buffer, buffer_size);
    gaincontrol.process(buffer, buffer_size);
    clipping.process(buffer, buffer_size);
    fmv.process(buffer, buffer_size);
    contour.process(buffer, buffer_size);

    for(std::size_t i = 0; i < buffer_size; i++)
    {
        buffer[i] *= volume;
    }
}

void Valvestate::prepare(float samplerate)
{
    input.prepare(samplerate);
    gaincontrol.prepare(samplerate);
    fmv.prepare(samplerate);
    contour.prepare(samplerate);
}

void Valvestate::reset(void)
{
    input.reset();
    gaincontrol.reset();
    fmv.reset();
    contour.reset();
}

}
}
}
