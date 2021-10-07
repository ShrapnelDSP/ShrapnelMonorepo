#include "abstract_dsp.h"
#include "juce_dsp/juce_dsp.h"
#include <assert.h>
#include <memory>

struct dspal_iir {
    std::unique_ptr<juce::dsp::IIR::Filter<float>> iir;
};

extern "C" {

dspal_err_t dspal_biquad_design_lowpass(float *coeffs, float f, float q_factor)
{
    // TODO check if the frequency is scaled correctly
    auto juce_coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(1, f, q_factor);
    auto raw_coeffs = juce_coeffs->getRawCoefficients();

    assert(2 == juce_coeffs->getFilterOrder());

    // These coefficients are not expected to be normalised
    assert(juce_coeffs->coefficients.size() == 6);
    assert(raw_coeffs[3] != 1);

    coeffs[0] = raw_coeffs[0]/raw_coeffs[3];
    coeffs[1] = raw_coeffs[1]/raw_coeffs[3];
    coeffs[2] = raw_coeffs[2]/raw_coeffs[3];
    coeffs[3] = raw_coeffs[4]/raw_coeffs[3];
    coeffs[4] = raw_coeffs[5]/raw_coeffs[3];

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

void dspal_iir_set_coeffs(dspal_iir_t iir, float *coeffs, size_t coeff_size)
{
    assert(coeff_size == 2);
    (void) coeff_size;

    iir->iir->coefficients = new juce::dsp::IIR::Coefficients<float>(
            coeffs[0], coeffs[1], coeffs[2], 1, coeffs[3], coeffs[4]);
}

void dspal_iir_process(dspal_iir_t iir, const float *in, float *out, size_t buf_size)
{
    auto in_block = juce::dsp::AudioBlock<const float>(&in, 1, 0, buf_size);
    auto out_block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
    auto context = juce::dsp::ProcessContextNonReplacing<float> (in_block, out_block);
    iir->iir->process(context);
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

};
