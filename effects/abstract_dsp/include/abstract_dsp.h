/** \file
 *
 * Abstraction layer for DSP functionality. This enables the use of use of
 * platform native, and potentially optimised DSP functions in effects in a
 * generic way.
 */

#pragma once

#include <stddef.h>

typedef struct dspal_iir *dspal_iir_t;

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
#ifdef __cplusplus
}
#endif
