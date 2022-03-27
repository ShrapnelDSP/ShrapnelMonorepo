#pragma once

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
        ESP_ERROR_CHECK(dsps_fft2r_init_fc32(table.data(), table.size()));
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

#if 0
        // transform a
        auto a_complex = real_to_complex(a);
        ESP_ERROR_CHECK(dsps_fft2r_fc32(a_complex.data(), N, table.data()));

        // transform b
        auto b_complex = real_to_complex(b);
        ESP_ERROR_CHECK(dsps_fft2r_fc32(b_complex.data(), N, table.data()));

        // multiply A * B
        std::array<float, 2*N> multiplied;
        complex_multiply(a_complex, b_complex, multiplied);

        // transform result
        ESP_ERROR_CHECK(dsps_fft2r_fc32(multiplied.data(), N, table.data()));

        complex_to_real(multiplied, out);
#endif
    }

    private:
    std::array<float, N> table;

    std::array<float, 2*N> real_to_complex(const std::array<float, N> &real)
    {
        std::array<float, 2*N> _complex;

        for(int i = 0; i < N; i++)
        {
            _complex[2*i] = real[i];
        }

        return _complex;
    }

    void complex_to_real(
            const std::array<float, 2*N> &_complex,
            std::array<float, N> &real)
    {
        for(int i = 0; i < N; i++)
        {
            real[i] = _complex[2*i];
        }
    }

    public:
    // TODO is const reference faster? This signature causes a copy of a and b
    // when the function is called.
    //
    // TODO std::complex should work here. It has reimplement cast that
    // allows casting array<complex<float>, N>.data() pointers to look like
    // a flat buffer with the real and imaginary parts interleaved, same as
    // the format used by ESP FFT
    static void complex_multiply(
            std::array<float, 2*N> a,
            std::array<float, 2*N> b,
            std::array<float, 2*N> &out)
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

        // part 1
        dsps_mul_f32(a.data(), b.data(), out.data(), N, 2, 2, 2);
        dsps_mul_f32(a.data(), b.data() + 1, out.data() + 1, N, 2, 2, 2);

        // part 2
        std::array<float, 2*N> out2;
        dsps_mul_f32(a.data() + 1, b.data() + 1, out2.data(), N, 2, 2, 2);
        dsps_mul_f32(a.data() + 1, b.data(), out2.data() + 1, N, 2, 2, 2);
        // TODO maybe faster if we use bit operations to inverse the sign bit?
        dsps_mulc_f32(out2.data(), out2.data(), N, -1, 2, 2);

        dsps_add_f32(out.data(), out2.data(), out.data(), out.size(), 1, 1, 1);
    }
};

}
}
