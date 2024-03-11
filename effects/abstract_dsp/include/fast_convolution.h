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
    FastConvolution(const std::array<float, K> &a)
    {
        // TODO Initialise FFT by hand to avoid some unnecessary allocations
        // and duplication of the twiddle factor buffer

        // TODO Instead of allocating input and output buffers, set the before
        // the fft_execute call as required at the call site. This avoids some
        // copies.
        fft_config = esp32_fft_init(
            N, ESP32_FFT_REAL, ESP32_FFT_FORWARD, nullptr, nullptr);
        ifft_config = esp32_fft_init(
            N, ESP32_FFT_REAL, ESP32_FFT_BACKWARD, nullptr, nullptr);

        assert(fft_config != nullptr);
        assert(ifft_config != nullptr);

        // transform a
        std::span<float, N> in{fft_config->input, N};
        std::fill(in.begin(), in.end(), 0);
        std::copy(a.cbegin(), a.cend(), fft_config->input);
        esp32_fft_execute(fft_config);
        std::span<float, N> out{fft_config->output, N};
        std::copy(out.begin(), out.end(), a_copy.begin());
    }

    ~FastConvolution()
    {
        esp32_fft_destroy(fft_config);
        esp32_fft_destroy(ifft_config);
    }

    /**
     * Process samples
     *
     * \f$ out = a \circledast b \f$
     */
    void process(const std::array<float, N> &b, std::array<float, N> &out)
    {
        profiling_mark_stage("convolution start");
        std::span<float, N> fft_in{fft_config->input, N};
        std::span<float, N> fft_out{fft_config->output, N};
        std::span<float, N> ifft_in{ifft_config->input, N};
        std::span<float, N> ifft_out{ifft_config->output, N};

        // transform b
        std::copy(b.begin(), b.end(), fft_in.begin());
        profiling_mark_stage("convolution b copy");
        esp32_fft_execute(fft_config);
        profiling_mark_stage("convolution b transform");

        // multiply A * B
        complex_multiply(a_copy.data(), fft_out.data(), ifft_in.data());
        profiling_mark_stage("convolution complex_multiply");

        // Inverse transform
        esp32_fft_execute(ifft_config);
        profiling_mark_stage("convolution ifft");
        std::copy(ifft_out.begin(), ifft_out.end(), out.begin());
        profiling_mark_stage("convolution out copy");
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
    std::array<float, N> a_copy;
    esp32_fft_config_t *fft_config;
    esp32_fft_config_t *ifft_config;
};

} // namespace shrapnel::dsp
