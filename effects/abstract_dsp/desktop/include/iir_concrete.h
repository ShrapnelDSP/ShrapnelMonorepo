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

#include "iir.h"
#include "juce_dsp/juce_dsp.h"
#include <array>

namespace shrapnel {
namespace dsp {

class IirFilter final : public IirFilterBase
{
public:
    IirFilter(void) { filter.reset(); }

    void process(const float *in, float *out, size_t buf_size) override
    {
        if(in == out)
        {
            auto block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
            auto context = juce::dsp::ProcessContextReplacing<float>(block);
            filter.process(context);
        }
        else
        {
            auto in_block =
                juce::dsp::AudioBlock<const float>(&in, 1, 0, buf_size);
            auto out_block = juce::dsp::AudioBlock<float>(&out, 1, 0, buf_size);
            auto context = juce::dsp::ProcessContextNonReplacing<float>(
                in_block, out_block);
            filter.process(context);
        }
    }

    void reset(void) override { filter.reset(); }

    void set_coefficients(std::array<float, 6> new_coefficients) override
    {
        filter.coefficients =
            new juce::dsp::IIR::Coefficients<float>(new_coefficients);
    }

    void set_coefficients(std::array<float, 8> new_coefficients) override
    {
        filter.coefficients =
            new juce::dsp::IIR::Coefficients<float>(new_coefficients);
    }

    void set_coefficients(std::array<float, 10> new_coefficients) override
    {
        filter.coefficients =
            new juce::dsp::IIR::Coefficients<float>(new_coefficients);
    }

private:
    juce::dsp::IIR::Filter<float> filter;
};

} // namespace dsp
} // namespace shrapnel
