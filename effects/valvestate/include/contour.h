
#pragma once
#include "iir_concrete.h"
#include <cstddef>

class ContourFilter
{
    public:
    ContourFilter();
    ~ContourFilter();

    void prepare(float samplerate);
    void process(float *buffer, std::size_t buffer_size);
    void reset(void);

    void set_parameter(float p);

    private:
    shrapnel::dsp::IirFilter filter;
    float samplerate;
};
