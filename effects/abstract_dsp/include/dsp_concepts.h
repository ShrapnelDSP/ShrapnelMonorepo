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
 */

#pragma once

#include <cstddef>
#include <span>

namespace {

template <typename T>
concept Resetable = requires(T processor) { processor.reset(); };

template <typename T>
concept Preparable = requires(T processor, float sample_rate, size_t max_size) {
                         processor.prepare(sample_rate, max_size);
                     };

template <typename T, size_t N>
concept Processable = requires(T processor, std::span<float, N> samples) {
                          processor.process(samples);
                      };

} // namespace

namespace shrapnel::dsp {

template <typename T, size_t N = std::dynamic_extent>
concept Processor = Resetable<T> && Preparable<T> && Processable<T, N>;

} // namespace shrapnel::dsp