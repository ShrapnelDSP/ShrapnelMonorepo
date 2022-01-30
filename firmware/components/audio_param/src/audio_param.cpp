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

/* \brief Audio parameter update functions 
 *
 * This file implements a generic audio parameter update function, to abstract
 * the details of individual parameter updates */

#include "audio_param.h"

namespace shrapnel {

AudioParameterFloat::AudioParameterFloat(
        std::string name,
        float minimum,
        float maximum,
        float default_value) :
    name(name),
    value(default_value),
    minimum(minimum),
    maximum(maximum) {}

void AudioParameterFloat::update(float value)
{
    if(value > 1)
    {
        return;
    }

    if(value < 0)
    {
        return;
    }

    auto range = maximum - minimum;
    this->value = minimum + value * range;
}

std::atomic<float> *AudioParameterFloat::get_raw_parameter(void)
{
    return &value;
}

AudioParameters::AudioParameters() {}

int AudioParameters::create_and_add_parameter(
        std::string name,
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

int AudioParameters::update(const std::string param, float value)
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

std::atomic<float> *AudioParameters::get_raw_parameter(const std::string param)
{
    auto element = parameters.find(param);

    if(element == parameters.end())
    {
        return nullptr;
    }

    return parameters[param]->get_raw_parameter();
}

}
