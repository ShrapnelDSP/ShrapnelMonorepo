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

#include "esp_dsp.h"
#include "profiling.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <complex>
#include <cstddef>

namespace shrapnel::dsp {

template <std::size_t N, std::size_t K>
    requires(CONFIG_DSP_MAX_FFT_SIZE == N / 2)
class FastConvolution final
{
public:
    // TODO move the definition of A into the prepare method
    // Resample A to the current sample rate in the prepare method
    FastConvolution(const std::array<float, K> &a)
    {
        esp_err_t rc = dsps_fft4r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
        assert(rc == ESP_OK);

        // transform a
        std::array<float, N> a_tmp{};
        std::copy(a.cbegin(), a.cend(), a_tmp.begin());
        rc = dsps_fft4r_fc32(reinterpret_cast<float *>(a_copy.data()), N / 2);
        assert(rc == ESP_OK);
        dsps_bit_rev4r_fc32(reinterpret_cast<float *>(a_copy.data()), N / 2);
        dsps_cplx2real_fc32(a_copy.data(), N / 2);
    }

    /**
     * Process samples
     *
     * \f$ out = a \circledast b \f$
     */
    void process(const std::array<float, N> &b, std::array<float, N> &out)
    {
        // transform b
        std::array<float, N> b_copy;
        std::copy(b.begin(), b.end(), b_copy.begin());
        profiling_mark_stage("convolution real_to_complex");
        int rc =
            dsps_fft4r_fc32(reinterpret_cast<float *>(b_copy.data()), N / 2);
        assert(rc == ESP_OK);
        profiling_mark_stage("convolution dsps_fft4r_fc32");
        dsps_bit_rev4r_fc32(reinterpret_cast<float *>(b_copy.data()), N / 2);
        profiling_mark_stage("convolution dsps_bit_rev4r_fc32");
        dsps_cplx2real_fc32(b_copy.data(), N / 2);
        profiling_mark_stage("convolution dsps_cplx2real_fc32");

        // multiply A * B
        // TODO put the complex multiply back here, the real transform still
        // produces complex numbers as the output. That's why the example app
        // does a[2*i + 0] ** 2 + a[2*i + 1] ** 2. It's getting the magnitude of
        // the frequency domain.

        std::array<float, N> multiplied;
        dsps_mul_f32_ae32(
            a_copy.data(), b_copy.data(), multiplied.data(), N, 1, 1, 1);
        profiling_mark_stage("convolution dsps_mul_f32_ae32");

        // TODO this may be incorrect. How to do a real only inverse transform?
        // http://www.dspguide.com/ch12/1.htm
        // http://www.dspguide.com/ch12/5.htm
        // we get even/odd decomposition for free when we run the forward
        // transform on a real buffer, due to the interlaced buffer format
        // re0, im0, re1, im1, etc is mapped to
        // re0, re1, re2, re3 of the real signal
        rc = dsps_fft4r_fc32(multiplied.data(), N / 2);
        assert(rc == ESP_OK);
        profiling_mark_stage("convolution dsps_fft4r_fc32");
        dsps_bit_rev4r_fc32(multiplied.data(), N / 2);
        profiling_mark_stage("convolution dsps_fft4r_fc32");
        dsps_cplx2real_fc32(multiplied.data(), N / 2);
        profiling_mark_stage("convolution dsps_cplx2real_fc32");

        // FIXME: skipping writing to out, because this version is generating NaNs
        dsps_mulc_f32(
            multiplied.data(), multiplied.data(), N, scale_factor, 1, 1);
        profiling_mark_stage("dsps_mulc_f32");

        // FIXME: bypass processing
        std::copy(b.begin(), b.end(), out.begin());
    }

private:
    static constexpr float scale_factor = 1.f / N;
    std::array<float, N> a_copy;
};

} // namespace shrapnel::dsp
