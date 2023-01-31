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

template<std::size_t N, std::size_t K>
class FastConvolution final {
    public:
    // TODO move the definition of A into the prepare method
    // Resample A to the current sample rate in the prepare method
    FastConvolution(const std::array<float, K> &a)
    {
        esp_err_t rc = dsps_fft4r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
        assert(rc == ESP_OK);

        // transform a
        std::array<float, N> a_copy{};
        std::copy(a.cbegin(), a.cend(), a_copy.begin());
        a_complex = real_to_complex(a_copy);
        rc = dsps_fft4r_fc32(reinterpret_cast<float *>(a_complex.data()), N);
        assert(rc == ESP_OK);
        dsps_bit_rev4r_fc32(reinterpret_cast<float *>(a_complex.data()), N);
    }

    /**
     * Process samples
     *
     * \f$ out = a \circledast b \f$
     */
    void process(
            const std::array<float, N> &b,
            std::array<float, N> &out)
    {
        // transform b
        auto b_complex = real_to_complex(b);
        profiling_mark_stage(7);
        int rc = dsps_fft4r_fc32(reinterpret_cast<float *>(b_complex.data()), N);
        assert(rc == ESP_OK);
        profiling_mark_stage(8);
        dsps_bit_rev4r_fc32(reinterpret_cast<float *>(b_complex.data()), N);
        profiling_mark_stage(9);

        // multiply A * B
        std::array<std::complex<float>, N> multiplied;
        complex_multiply(a_complex, b_complex, multiplied);
        profiling_mark_stage(10);

        // transform result
        // Inverse transform achieved by complex conjugating the input and
        // output of the forward transform. The imaginary part of the output is
        // not used, so we don't actually calculate the complex conjugate on
        // it.
        //
        // There is no inverse transform provided by esp-dsp.
        auto multiplied_ptr = reinterpret_cast<float *>(multiplied.data());
        dsps_mulc_f32(multiplied_ptr + 1, multiplied_ptr + 1, N, -1, 2, 2);
        profiling_mark_stage(11);
        rc = (dsps_fft4r_fc32(multiplied_ptr, N));
        assert(rc == ESP_OK);
        profiling_mark_stage(12);
        dsps_bit_rev4r_fc32(multiplied_ptr, N);
        profiling_mark_stage(13);

        complex_to_real(multiplied, out);
        profiling_mark_stage(14);

        dsps_mulc_f32(out.data(), out.data(), N, scale_factor, 1, 1);
        profiling_mark_stage(15);
    }

    private:
    static constexpr float scale_factor = 1.f/N;
    std::array<std::complex<float>, N> a_complex;

    std::array<std::complex<float>, N> real_to_complex(const std::array<float, N> &real)
    {
        std::array<std::complex<float>, N> _complex{};

        for(std::size_t i = 0; i < N; i++)
        {
            _complex[i] = real[i];
        }

        return _complex;
    }

    void complex_to_real(
            const std::array<std::complex<float>, N> &_complex,
            std::array<float, N> &real)
    {
        for(std::size_t i = 0; i < N; i++)
        {
            real[i] = _complex[i].real();
        }
    }

    public:
    static void complex_multiply(
            const std::array<std::complex<float>, N> &a,
            const std::array<std::complex<float>, N> &b,
            std::array<std::complex<float>, N> &out)
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

        // TODO we multiply then add here, could we use the MADD.S instruction
        // to speed it up?

        auto a_ptr = reinterpret_cast<const float*>(a.data());
        auto b_ptr = reinterpret_cast<const float*>(b.data());
        auto out_ptr = reinterpret_cast<float*>(out.data());
        // part 1
        dsps_mul_f32(a_ptr, b_ptr, out_ptr, N, 2, 2, 2);
        dsps_mul_f32(a_ptr, b_ptr + 1, out_ptr + 1, N, 2, 2, 2);

        // part 2
        std::array<std::complex<float>, N> out2;
        auto out2_ptr = reinterpret_cast<float*>(out2.data());
        dsps_mul_f32(a_ptr + 1, b_ptr + 1, out2_ptr, N, 2, 2, 2);
        dsps_mul_f32(a_ptr + 1, b_ptr, out2_ptr + 1, N, 2, 2, 2);
        dsps_mulc_f32(out2_ptr, out2_ptr, N, -1, 2, 2);

        dsps_add_f32(out_ptr, out2_ptr, out_ptr, 2*out.size(), 1, 1, 1);
    }
};

}
