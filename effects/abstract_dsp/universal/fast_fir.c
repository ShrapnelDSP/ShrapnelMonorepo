#include "fast_fir.h"

#include "string.h"

static float *coefficients;
static size_t coefficients_length;

static float *input_samples;
static size_t input_length;

static size_t block_length = a_block_length;

void dspal_fast_fir_init(float *a_coefficients, size_t a_coefficients_length,
                         float *buffer, size_t buffer_length,
                         size_t a_block_length)
{
    coefficients = a_coefficients;
    coefficients_length = a_coefficients_length;

    input_samples = buffer;
    input_length = buffer_length;

    block_length = a_block_length;

    dspal_fast_fir_reset();
}

void dspal_fast_fir_process(float *buffer)
{
    memmove(&input_samples[0], &input_samples[block_length],
            block_length*sizeof(*input_samples));
    memcpy(&input_samples[input_length - block_length - 1], buffer,
            block_length*sizeof(*input_samples));

    float conv_out[coefficients_length + block_length - 1];

    //fft_convolution();
}

void dspal_fast_fir_reset(void)
{
    memset(input_samples, 0, input_length*sizeof(*input_samples));
}
