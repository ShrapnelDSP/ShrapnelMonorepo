#pragma once

#include <complex>
#include <cstddef>
#include <array>
#include "esp_dsp.h"

namespace shrapnel {
namespace dsp {

template<std::size_t N>
class FastConvolution final {
    public:
    FastConvolution()
    {
        ESP_ERROR_CHECK(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE));
    }

    /**
     * Process samples
     *
     * \f$ out = a \circledast b \f$
     */
    void process(
            const std::array<float, N> &a,
            const std::array<float, N> &b,
            std::array<float, N> &out)
    {
        // TODO check the esp-dsp example to see how it's done. They have a
        // define using a global table pointer, how are we using that? What's
        // the point of the table passed to the init function?

        // transform a
        auto a_complex = real_to_complex(a);
        ESP_ERROR_CHECK(dsps_fft2r_fc32(reinterpret_cast<float *>(a_complex.data()), N));
        dsps_bit_rev_fc32(reinterpret_cast<float *>(a_complex.data()), N);

        // transform b
        auto b_complex = real_to_complex(b);
        ESP_ERROR_CHECK(dsps_fft2r_fc32(reinterpret_cast<float *>(b_complex.data()), N));
        dsps_bit_rev_fc32(reinterpret_cast<float *>(b_complex.data()), N);

        // multiply A * B
        std::array<std::complex<float>, N> multiplied;
        complex_multiply(a_complex, b_complex, multiplied);

        std::array<std::complex<float>, N> test{};
        for(auto &t : test)
        {
            t = 1;
        }
        multiplied = test;

        int i = 0;
        printf("\nmultiplied\n");
        for(const auto &n: multiplied)
        {
            printf("n %d %f %f\n", i, n.real(), n.imag());
            i++;
        }

        // transform result
        ESP_ERROR_CHECK(dsps_fft2r_fc32(reinterpret_cast<float *>(multiplied.data()), N));
        dsps_bit_rev_fc32(reinterpret_cast<float *>(multiplied.data()), N);

        i = 0;
        printf("\ntransformed\n");
        for(const auto &n: multiplied)
        {
            printf("n %d %f %f\n", i, n.real(), n.imag());
            i++;
        }

        complex_to_real(multiplied, out);
        dsps_mulc_f32(out.data(), out.data(), N, scale, 1, 1);

        i = 0;
        printf("\nscaled\n");
        for(const auto &n: out)
        {
            printf("n %d %f\n", i, n);
            i++;
        }
    }

    private:
    float scale = 1.f/N;

    std::array<std::complex<float>, N> real_to_complex(const std::array<float, N> &real)
    {
        std::array<std::complex<float>, N> _complex{};

        for(int i = 0; i < N; i++)
        {
            _complex[i] = real[i];
        }

        return _complex;
    }

    void complex_to_real(
            const std::array<std::complex<float>, N> &_complex,
            std::array<float, N> &real)
    {
        for(int i = 0; i < N; i++)
        {
            real[i] = _complex[i].real();
        }
    }

    public:
    // TODO is const reference faster? This signature causes a copy of a and b
    // when the function is called.
    static void complex_multiply(
            std::array<std::complex<float>, N> a,
            std::array<std::complex<float>, N> b,
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

        auto a_ptr = reinterpret_cast<float*>(a.data());
        auto b_ptr = reinterpret_cast<float*>(b.data());
        auto out_ptr = reinterpret_cast<float*>(out.data());
        // part 1
        dsps_mul_f32(a_ptr, b_ptr, out_ptr, N, 2, 2, 2);
        dsps_mul_f32(a_ptr, b_ptr + 1, out_ptr + 1, N, 2, 2, 2);

        // part 2
        std::array<std::complex<float>, N> out2;
        auto out2_ptr = reinterpret_cast<float*>(out2.data());
        dsps_mul_f32(a_ptr + 1, b_ptr + 1, out2_ptr, N, 2, 2, 2);
        dsps_mul_f32(a_ptr + 1, b_ptr, out2_ptr + 1, N, 2, 2, 2);
        // TODO maybe faster if we use bit operations to inverse the sign bit?
        dsps_mulc_f32(out2_ptr, out2_ptr, N, -1, 2, 2);

        dsps_add_f32(out_ptr, out2_ptr, out_ptr, 2*out.size(), 1, 1, 1);
    }
};

}
}
