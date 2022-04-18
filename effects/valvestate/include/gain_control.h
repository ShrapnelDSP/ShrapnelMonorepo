
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
