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
#include "dsps_mulc.h"

namespace shrapnel::effect::valvestate {

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

void Valvestate::process(std::span<float> buffer)
{
    input.process(buffer);
    gaincontrol.process(buffer);
    clipping.process(buffer);
    fmv.process(buffer);
    contour.process(buffer);

    for(float &sample : buffer)
    {
        sample *= volume;
    }
}

void Valvestate::prepare(float samplerate, size_t size)
{
    input.prepare(samplerate, size);
    gaincontrol.prepare(samplerate, size);
    fmv.prepare(samplerate, size);
    contour.prepare(samplerate, size);
}

void Valvestate::reset()
{
    input.reset();
    gaincontrol.reset();
    fmv.reset();
    contour.reset();
}

}
