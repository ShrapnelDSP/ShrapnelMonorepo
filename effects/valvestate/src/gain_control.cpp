
#include "gain_control.h"
#include <cmath>

namespace shrapnel {
namespace effect {
namespace valvestate {

void GainControl::set_parameters(float g, float channel)
{
    float B0, B1, B2, A0, A1, A2;
    float K = 2*samplerate;

    if (channel < 0.5f)
    {
        //OD1
        B0 = 0.00916582638188652f*K*g;
        B1 = 0;
        B2 = -0.00916582638188652f*K*g;

        A0 = 4.30793839948666e-8f*std::pow(K, 2.f)*g + 2.35e-5f*K*g +
            0.000916582638188652f*K + 0.5f;
        A1 = -8.61587679897332e-8f*std::pow(K, 2.f)*g + 1.0f;
        A2 = 4.30793839948666e-8f*std::pow(K, 2.f)*g - 2.35e-5f*K*g -
            0.000916582638188652f*K + 0.5f;
    }
    else
    {
        //OD2
        B0 = 0.11f*K*g;
        B1 = 0;
        B2 = -0.11f*K*g;

        A0 = 4.69953e-8f*std::pow(K, 2.f)*g + 2.35e-5f*K*g + 0.0009999f*K + 0.5f;
        A1 = -9.39906e-8f*std::pow(K, 2.f)*g + 1.0f;
        A2 = 4.69953e-8f*std::pow(K, 2.f)*g - 2.35e-5f*K*g - 0.0009999f*K + 0.5f;
    }

    filter.set_coefficients(std::array<float, 6>({B0, B1, B2, A0, A1, A2}));
}

void GainControl::prepare(float a_samplerate)
{
    filter.reset();
    samplerate = a_samplerate;
}

void GainControl::process(float *buffer, std::size_t buffer_size)
{
    filter.process(buffer, buffer, buffer_size);
}

void GainControl::reset(void)
{
    filter.reset();
}

}
}
}
