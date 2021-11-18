#include "iir.h"

void iir_process(const float *input, float *output, int len, float *coeffs, float *w, int coeffs_len)
{
    float out_tmp;
    float *b = coeffs;
    float *a = &coeffs[coeffs_len/2];
    for(int i = 0; i < len; i++)
    {
        //calculate output
        out_tmp = (w[0] + b[0] * input[i])/a[0];

        for(int j = 0; j < coeffs_len/2 - 1; j++)
        {
            //update the state
            w[j] = 0;
            w[j] -= a[j+1] * out_tmp;
            w[j] += b[j+1] * input[i];
            /* add the next state variable down, except when at the last one */
            if(j < coeffs_len/2 - 2)
            {
                w[j] += w[j+1];
            }
        }
        output[i] = out_tmp;
    }
}
