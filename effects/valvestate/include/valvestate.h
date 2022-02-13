#pragma once

#include <cstddef>
#include "input_filter.h"
#include "gain_control.h"
#include "clipping.h"
#include "fmv.h"
#include "contour.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

class Valvestate
{
    public:
    Valvestate();

    void set_gain(float gain, float channel);

    void set_fmv(float bass, float middle, float treble);

    void set_contour(float contour);

    void set_volume(float volume);

    void process(float *buffer, std::size_t buffer_size);

    void prepare(float samplerate);
    void reset(void);

    private:
    InputFilter input;
    GainControl gaincontrol;
    Clipping clipping;
    FMVFilter fmv;
    ContourFilter contour;
    float volume;
};

}
}
}
