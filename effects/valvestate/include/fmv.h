
#pragma once

#include "iir_concrete.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

class FMVFilter
{
    public:
    void set_parameters(float l, float m, float t);

    void prepare(float samplerate);
    void process(float *buffer, std::size_t buffer_size);
    void reset();

    private:
    shrapnel::dsp::IirFilter filter;
    float samplerate;
};

}
}
}
