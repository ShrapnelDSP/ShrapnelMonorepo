
#include "fmv.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

void FMVFilter::prepare(float a_samplerate)
{
    filter.reset();
    samplerate = a_samplerate;
}

void FMVFilter::process(float *buffer, std::size_t buffer_size)
{
    filter.process(buffer, buffer, buffer_size);
}

void FMVFilter::reset()
{
    filter.reset();
}

void FMVFilter::set_parameters(float l, float m, float t)
{
    const float R1 = 220e3;
    const float R2 = 1e6;
    const float R3 = 22e3;
    const float R4 = 33e3;

    const float C1 = 470e-12;
    const float C2 = 22e-9;
    const float C3 = 22e-9;

    float b1 = t * C1 * R1 + m * C3 * R3 + l * (C1 * R2 + C2 * R2) + 
        (C1 * R3 + C2 * R3);

    float b2 = t * (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 ) 
        - m * m * (C1 * C3 * R3 * R3  + C2 * C3 * R3 * R3 )
        + m * (C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3)
        + l * (C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4)
        + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3)
        + (C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4);

    float b3 = l*m*(C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4 )
        - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4 )
        + m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4 )
        + t * C1 * C2 * C3 * R1 * R3 * R4 - t * m * C1 * C2 * C3 * R1 * R3 * R4
        + t * l * C1 * C2 * C3 * R1 * R2 * R4;

    float a0 = 1;

    float a1 = (C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4 )
        + m * C3 * R3 + l * (C1 * R2 + C2 * R2);

    float a2 = m * (C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3
        + C2 * C3 * R3 * R3 ) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3 )
        - m*m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3 ) + l*(C1 * C2 * R2 * R4
        + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4 )
        + (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4
        + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4 );

    float a3 = l*m*(C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4 )
        - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4)
        + m * (C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 
                - C1 * C2 * C3 * R1 * R3 * R4 ) 
        + l * C1 * C2 * C3 * R1 * R2 * R4 + C1 * C2 * C3 * R1 * R3 * R4;

    //bilinear transform
    float c = 2*samplerate;
    float c2 = c*c;
    float c3 = c*c*c;

    float  B0 = -b1 * c- b2 * c2 - b3 * c3;
    float  B1 = -b1 * c+ b2 * c2 + 3 * b3 * c3;
    float  B2 = b1 * c+ b2 * c2 - 3 * b3 * c3;
    float  B3 = b1 * c- b2 * c2 + b3 * c3;

    float  A0 = -a0 - a1 * c- a2 * c2 - a3 * c3;
    float  A1 = -3 * a0 - a1 * c+ a2 * c2 + 3 * a3 * c3;
    float  A2 = -3 * a0 + a1 * c+ a2 * c2 - 3 * a3 * c3;
    float  A3 = -a0 + a1 * c- a2 * c2 + a3 * c3;

    filter.set_coefficients(std::array<float, 8>{B0, B1, B2, B3, A0, A1, A2, A3});
}

}
}
}
