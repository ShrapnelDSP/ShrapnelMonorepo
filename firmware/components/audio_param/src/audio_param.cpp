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

#include "audio_param.h"

namespace shrapnel {
namespace parameters {

AudioParameterFloat::AudioParameterFloat(
        const id_t &a_name,
        float a_minimum,
        float a_maximum,
        float default_value) :
    name(a_name),
    value(default_value),
    minimum(a_minimum),
    maximum(a_maximum) {}

void AudioParameterFloat::update(float a_value)
{
    if(a_value > 1)
    {
        return;
    }

    if(a_value < 0)
    {
        return;
    }

    auto range = maximum - minimum;
    value = minimum + a_value * range;
}

float AudioParameterFloat::get(void)
{
    auto range = maximum - minimum;
    return (value - minimum) / range;
}

std::atomic<float> *AudioParameterFloat::get_raw_parameter(void)
{
    return &value;
}

AudioParameters::AudioParameters() {}

int AudioParameters::create_and_add_parameter(
        const id_t &name,
        float minimum,
        float maximum,
        float default_value)
{
    if(parameters.full())
    {
        return -1;
    }

    parameters[name] = std::make_unique<AudioParameterFloat>(name, minimum, maximum, default_value);

    return 0;
}

int AudioParameters::update(const id_t &param, float value)
{
    (void) param;
    (void) value;

    auto element = parameters.find(param);

    if(element == parameters.end())
    {
        return -1;
    }

    parameters[param]->update(value);

    return 0;
}

float AudioParameters::get(const id_t &param)
{
    auto element = parameters.find(param);

    if(element == parameters.end())
    {
        return 0;
    }

    return parameters[param]->get();
}

std::atomic<float> *AudioParameters::get_raw_parameter(const id_t &param)
{
    auto element = parameters.find(param);

    if(element == parameters.end())
    {
        return nullptr;
    }

    return parameters[param]->get_raw_parameter();
}

AudioParameters::MapType::iterator AudioParameters::begin()
{
    return parameters.begin();
}

AudioParameters::MapType::iterator AudioParameters::end()
{
    return parameters.end();
}

}
}
