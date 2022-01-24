#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void iir_process(const float *input, float *output, int len, float *coeffs, float *w, int coeffs_len);

#ifdef __cplusplus
}
#endif
