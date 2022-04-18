
#include "input_filter.h"
#include <cmath>

namespace shrapnel {
namespace effect {
namespace valvestate {

void InputFilter::prepare(float samplerate)
{
    float K = 2 * samplerate;
    float B0 = 1.10449884469421e-9f*std::pow(K, 2.f) + 0.00246545127613797f*K + 0.5f;
    float B1 = 1.0f - 2.20899768938842e-9f*std::pow(K, 2.f);
    float B2 = 1.10449884469421e-9f*std::pow(K, 2.f) - 0.00246545127613797f*K + 0.5f;

    float A0 = 1.10449884469421e-9f*std::pow(K, 2.f) + 0.000115449950739352f*K + 0.5f;
    float A1 = 1.0f - 2.20899768938842e-9f*std::pow(K, 2.f);
    float A2 = 1.10449884469421e-9f*std::pow(K, 2.f) - 0.000115449950739352f*K + 0.5f;

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
