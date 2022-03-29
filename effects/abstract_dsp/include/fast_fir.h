/** \file
 *
 * Fast FIR filter implemented using FFT
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

namespace shrapnel {
namespace dsp {

/**
 * \tparam N The number of samples in each buffer passed to \ref process()
 * \tparam M The number of samples used for convolution
 * \tparam K The number of samples used for coefficients
 * \tparam Convolution The circular convolution processor to use
 */
template<size_t N, size_t M, size_t K, typename Convolution>
class FastFir final {
    // TODO add static asserts for the following:
    // buffer size is power of two
    // convolution size is power of two
    // Coefficients size is not more than convolutions size / 2

    public:
    /** Initialise the filter
     *
     * \param coefficients A buffer containing the filter coefficients
     */
    FastFir(const std::array<float, K> &coefficients, Convolution &convolution) :
        signal{}, coefficients{}, convolution{convolution}
    {
        std::copy(coefficients.cbegin(), coefficients.cend(), this->coefficients.begin());
    }

    /** Filter samples
     *
     * \param[in,out] buffer Samples to process. Must be \p N samples long.
     */
    void process(float *buffer)
    {
        std::copy(signal.begin() + N, signal.end(), signal.begin());
        std::copy(buffer, buffer + N, signal.end() - N);

        std::array<float, M> out;
        convolution.process(coefficients, signal, out);
    }

    /** Reset the state of the filter
     *
     * \note Does not reset coefficients
     */
    void reset(void);

    private:
    // TODO this is not perfect, the convolution could become invalid at any
    // time.  We can't use std::unique_ptr, since the tests need access to
    // convolution to set up expectations.
    //
    // Can we set up all expectations before we even the convolution to this
    // class? In that case unique_ptr is fine.
    Convolution &convolution;
    std::array<float, M> coefficients;
    std::array<float, M> signal;
};

}
}
