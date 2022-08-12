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
