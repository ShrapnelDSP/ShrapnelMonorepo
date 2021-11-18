#pragma once

void iir_process(const float *input, float *output, int len, float *coeffs, float *w, int coeffs_len);
