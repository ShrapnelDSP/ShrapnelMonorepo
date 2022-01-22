#include "abstract_dsp.h"
#include "juce_dsp/juce_dsp.h"
#include "delayline.h"
#include <assert.h>
#include <memory>

struct dspal_iir {
    std::unique_ptr<juce::dsp::IIR::Filter<float>> iir;
};

struct dspal_delayline {
    std::unique_ptr<shrapnel::dsp::DelayLine> delayline;
};

extern "C" {

dspal_err_t dspal_biquad_design_lowpass(float *coeffs, float f, float q_factor)
{
    // TODO check if the frequency is scaled correctly
    auto juce_coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(1, f, q_factor);
    auto raw_coeffs = juce_coeffs->getRawCoefficients();

    assert(2 == juce_coeffs->getFilterOrder());

    // These coefficients are expected to be normalised
    assert(5 == juce_coeffs->coefficients.size());

    memcpy(coeffs, raw_coeffs, 5*sizeof(float));

    return DSPAL_OK;
}

dspal_iir_t dspal_iir_create(size_t order)
{
    // JUCE IIR filters can change their order on the fly to match the selected
    // coefficients
    (void) order;

    dspal_iir_t iir = new dspal_iir;

    iir->iir.reset(new juce::dsp::IIR::Filter<float>());
    return iir;
}

void dspal_iir_set_coeffs(dspal_iir_t iir, const float *coeffs, size_t coeff_size)
{
    assert(coeff_size == 2);
    assert(iir != NULL);
    (void) coeff_size;

    iir->iir->coefficients = new juce::dsp::IIR::Coefficients<float>(
            coeffs[0], coeffs[1], coeffs[2], 1, coeffs[3], coeffs[4]);
}

void dspal_iir_process(dspal_iir_t iir, const float *in, float *out, size_t buf_size)
{
    if(in == out)
    {
        auto block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
        auto context = juce::dsp::ProcessContextReplacing<float> (block);
        iir->iir->process(context);
    }
    else
    {
        auto in_block = juce::dsp::AudioBlock<const float>(&in, 1, 0, buf_size);
        auto out_block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
        auto context = juce::dsp::ProcessContextNonReplacing<float> (in_block, out_block);
        iir->iir->process(context);
    }
}

void dspal_iir_reset(dspal_iir_t iir)
{
    iir->iir->reset();
}

void dspal_multiply(const float *in1, const float *in2, float *out, size_t buf_size)
{
    for(size_t i = 0; i < buf_size; i++)
    {
        out[i] = in1[i] * in2[i];
    }
}

dspal_delayline_t dspal_delayline_create(size_t max_samples)
{
    dspal_delayline_t delayline = new dspal_delayline;

    delayline->delayline.reset(new shrapnel::dsp::DelayLine(max_samples));

    return delayline;
}

void dspal_delayline_destroy(dspal_delayline_t delayline)
{
    delete delayline;
}

void dspal_delayline_set_delay(dspal_delayline_t delayline, float delay)
{
    delayline->delayline->set_delay(delay);
}

void dspal_delayline_push_sample(dspal_delayline_t delayline, float sample)
{
    delayline->delayline->push_sample(sample);
}

float dspal_delayline_pop_sample(dspal_delayline_t delayline)
{
    return delayline->delayline->pop_sample();
}

};
