/** \file
 *
 * Abstraction layer for DSP functionality. This enables the use of use of
 * platform native, and potentially optimised DSP functions in effects in a
 * generic way.
 */

#pragma once

#include <stddef.h>

typedef struct dspal_iir *dspal_iir_t;
typedef struct dspal_delayline *dspal_delayline_t;

typedef enum {
    DSPAL_OK,
    DSPAL_FAIL,
} dspal_err_t;

#ifdef __cplusplus
extern "C" {
#endif

dspal_err_t dspal_biquad_design_lowpass(float *coeffs, float f, float q_factor);

dspal_iir_t dspal_iir_create(size_t order);
void dspal_iir_set_coeffs(dspal_iir_t iir, const float *coeffs, size_t coeff_size);
void dspal_iir_process(dspal_iir_t iir, const float *in, float *out, size_t buf_size);
void dspal_iir_reset(dspal_iir_t iir);

void dspal_multiply(const float *in1, const float *in2, float *out, size_t buf_size);

dspal_delayline_t dspal_delayline_create(size_t max_samples);
void dspal_delayline_set_delay(dspal_delayline_t delayline, float delay);
void dspal_delayline_set_buffer_size(dspal_delayline_t delayline, size_t size);
void dspal_delayline_push_sample(dspal_delayline_t delayline, float sample);
float dspal_delayline_pop_sample(dspal_delayline_t delayline);

#ifdef __cplusplus
}
#endif
