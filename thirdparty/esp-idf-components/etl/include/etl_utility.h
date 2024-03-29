#pragma once

#include "etl/string_stream.h"
#include <utility>
#include <array>
#include <optional>

namespace etl {

template<typename T1, typename T2>
etl::string_stream& operator<<(etl::string_stream& out, const std::pair<T1, T2>& pair)
{
    out << "{ " << pair.first << ", " << pair.second << " }";
    return out;
}

template<typename T, std::size_t MAX_SIZE>
etl::string_stream& operator<<(etl::string_stream& out, const std::array<T, MAX_SIZE>& array)
{
    out << "[ ";
    for(const auto &value : array)
    {
        out << value << " ";
    }
    out << "]";
    return out;
}

template <typename T>
etl::string_stream &operator<<(etl::string_stream &out,
                               const std::optional<T> &self)
{
    out << "optional ";
    if(self.has_value())
    {
        out << "with value " << *self;
    }
    else
    {
        out << "with no value";
    }
    return out;
}

}
