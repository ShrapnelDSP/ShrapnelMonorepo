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

#include "dsp_concepts.h"
#include "profiling.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>

namespace {
template <typename T, T value>
concept integral_power_of_2 =
    std::unsigned_integral<T> && requires { value && !(value & (value - 1)); };
}

namespace shrapnel::dsp {

/**
 * \tparam N The number of samples in each buffer passed to \ref process()
 * \tparam M The number of samples used for convolution
 * \tparam Convolution The circular convolution processor to use
 */
template <size_t N, size_t M, typename Convolution>
    requires integral_power_of_2<decltype(N), N> &&
             integral_power_of_2<decltype(M), M> && requires { M <= N / 2; }
class FastFir final
{
public:
    /** Initialise the filter
     */
    explicit FastFir(std::unique_ptr<Convolution> a_convolution)
        requires(dsp::Processor<FastFir<N, M, Convolution>, N>)
        : signal{},
          convolution{std::move(a_convolution)}
    {
    }

    // TODO implement variable buffer size processing
    void prepare(float, size_t){};

    /** Filter samples
     *
     * \param[in,out] buffer Samples to process. Must be \p N samples long.
     */
    void process(std::span<float, N> buffer)
    {
        std::copy(signal.begin() + N, signal.end(), signal.begin());
        profiling_mark_stage(5);
        std::copy(buffer.data(), buffer.data() + N, signal.end() - N);
        profiling_mark_stage(6);

        std::array<float, M> out;
        convolution->process(signal, out);
        profiling_mark_stage(16);

        std::copy(out.end() - N, out.end(), buffer.data());
    }

    /** Reset the state of the filter
     *
     * \note Does not reset coefficients
     */
    void reset() { signal.fill(0); };

private:
    std::array<float, M> signal;
    std::unique_ptr<Convolution> convolution;
};

} // namespace shrapnel::dsp
