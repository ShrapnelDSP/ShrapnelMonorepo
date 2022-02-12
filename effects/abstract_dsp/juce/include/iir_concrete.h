#pragma once

#include <array>
#include "iir.h"
#include "juce_dsp/juce_dsp.h"

namespace shrapnel {
namespace dsp {

class IirFilter final : public IirFilterBase {
    public:
    IirFilter(void)
    {
        filter.reset();
    }

    void process(const float *in, float *out, size_t buf_size) override
    {
        if(in == out)
        {
            auto block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
            auto context = juce::dsp::ProcessContextReplacing<float> (block);
            filter.process(context);
        }
        else
        {
            auto in_block = juce::dsp::AudioBlock<const float>(&in, 1, 0, buf_size);
            auto out_block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
            auto context = juce::dsp::ProcessContextNonReplacing<float> (in_block, out_block);
            filter.process(context);
        }
    }

    void reset(void) override
    {
        filter.reset();
    }

    void set_coefficients(std::array<float, 6> new_coefficients) override
    {
        filter.coefficients =
            new juce::dsp::IIR::Coefficients<float>(new_coefficients);
    }

    private:
    juce::dsp::IIR::Filter<float> filter;
};

}
}
