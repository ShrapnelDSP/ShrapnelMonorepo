/** \file
 *
 * Fast FIR filter implemented using FFT
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>

void profiling_mark_stage(unsigned int stage);

namespace shrapnel {
namespace dsp {

/**
 * \tparam N The number of samples in each buffer passed to \ref process()
 * \tparam M The number of samples used for convolution
 * \tparam Convolution The circular convolution processor to use
 */
template<size_t N, size_t M, typename Convolution>
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
    FastFir(std::unique_ptr<Convolution> a_convolution) : signal{}, convolution{std::move(a_convolution)} {}

    /** Filter samples
     *
     * \param[in,out] buffer Samples to process. Must be \p N samples long.
     */
    void process(float *buffer)
    {
        std::copy(signal.begin() + N, signal.end(), signal.begin());
        profiling_mark_stage(5);
        std::copy(buffer, buffer + N, signal.end() - N);
        profiling_mark_stage(6);

        std::array<float, M> out;
        convolution->process(signal, out);
        profiling_mark_stage(16);

        std::copy(out.end() - N, out.end(), buffer);
    }

    /** Reset the state of the filter
     *
     * \note Does not reset coefficients
     */
    void reset(void);

    private:
    std::array<float, M> signal;
    std::unique_ptr<Convolution> convolution;
};

}
}
