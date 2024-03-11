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
 *
 */

#pragma once

#include "esp32_fft.h"
#include "profiling.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <complex>
#include <cstddef>
#include <esp_dsp.h>
#include <span>

namespace shrapnel::dsp {

template <std::size_t N, std::size_t K>
// The FFT library doesn't seem to work when using fewer than 8 points
    requires(N >= 8)
class FastConvolution final
{
public:
    // TODO move the definition of A into the prepare method
    // Resample A to the current sample rate in the prepare method
    explicit FastConvolution(const std::array<float, K> &a)
    {
        // Initialise FFT structs by hand to avoid some unnecessary allocations
        // and duplication of the twiddle factor buffer. Input and output
        // buffers are assigned later to avoid some allocations and copies
        // from the FFT buffer to the output buffer of the functions.
        constexpr float two_pi = 6.28318530;
        constexpr float two_pi_by_n = two_pi / N;

        int k, m;
        for(k = 0, m = 0; k < N; k++, m += 2)
        {
            twiddle_factors[m] = cosf(two_pi_by_n * k);     // real
            twiddle_factors[m + 1] = sinf(two_pi_by_n * k); // imag
        }

        fft_config.flags = 0;
        fft_config.type = ESP32_FFT_REAL;
        fft_config.direction = ESP32_FFT_FORWARD;
        fft_config.size = N;
        fft_config.twiddle_factors = twiddle_factors.data();

        ifft_config.flags = 0;
        ifft_config.type = ESP32_FFT_REAL;
        ifft_config.direction = ESP32_FFT_BACKWARD;
        ifft_config.size = N;
        ifft_config.twiddle_factors = twiddle_factors.data();

        // transform a
        std::array<float, N> a_tmp{};
        std::copy(a.cbegin(), a.cend(), a_tmp.data());
        fft_config.input = a_tmp.data();
        fft_config.output = a_copy.data();
        esp32_fft_execute(&fft_config);
    }

    /**
     * Process samples
     *
     * \f$ out = a \circledast b \f$
     */
    void process(const std::array<float, N> &b, std::array<float, N> &out)
    {
        profiling_mark_stage("convolution start");

        // transform b
        // const cast is OK because the forward FFT does not modify the input
        // buffer
        fft_config.input = const_cast<float *>(b.data());
        fft_config.output = out.data();
        esp32_fft_execute(&fft_config);
        profiling_mark_stage("convolution b transform");

        // multiply A * B
        std::array<float, N> multiplied;
        complex_multiply(a_copy.data(), out.data(), multiplied.data());
        profiling_mark_stage("convolution complex_multiply");

        // Inverse transform
        ifft_config.input = multiplied.data();
        ifft_config.output = out.data();
        esp32_fft_execute(&ifft_config);
        profiling_mark_stage("convolution ifft");
    }

    static void complex_multiply(const float *a, const float *b, float *out)
    {
        // We want to compute (r_a + im_a j) * (r_b + im_b j)
        // This can be rewritten as follows:
        //
        // ra rb + ra im_b j + im_a r_b j + im_a im_b j j
        //
        // j j  is -1 so:
        //
        // (ra rb + ra im_b j) +    // part 1
        // (- im_a im_b + im_a r_b j)   // part 2

        // The FFT result has a special encoding for DC and Nyquist. DC is
        // stored in the first element, and nyquist in the second.
        out[0] = a[0] * b[0];
        out[1] = a[1] * b[1];

        a += 2;
        b += 2;
        out += 2;

        // TODO we multiply then add here, could we use the MADD.S instruction
        // to speed it up?

        // part 1
        dsps_mul_f32(a, b, out, N / 2 - 1, 2, 2, 2);
        dsps_mul_f32(a, b + 1, out + 1, N / 2 - 1, 2, 2, 2);

        // part 2
        std::array<float, N - 2> out2;
        auto out2_ptr = reinterpret_cast<float *>(out2.data());
        dsps_mul_f32(a + 1, b + 1, out2_ptr, N / 2 - 1, 2, 2, 2);
        dsps_mul_f32(a + 1, b, out2_ptr + 1, N / 2 - 1, 2, 2, 2);
        dsps_mulc_f32(out2_ptr, out2_ptr, N / 2 - 1, -1, 2, 2);

        dsps_add_f32(out, out2_ptr, out, N - 2, 1, 1, 1);
    }

private:
    std::array<float, 2 * N> twiddle_factors;
    std::array<float, N> a_copy;
    esp32_fft_config_t fft_config;
    esp32_fft_config_t ifft_config;
};

} // namespace shrapnel::dsp
