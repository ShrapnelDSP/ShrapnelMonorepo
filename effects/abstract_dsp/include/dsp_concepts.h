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