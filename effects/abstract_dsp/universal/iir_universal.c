/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#include "iir_universal.h"

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
