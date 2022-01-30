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
