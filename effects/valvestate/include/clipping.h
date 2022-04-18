
#pragma once

#include <cstddef>

namespace shrapnel {
namespace effect {
namespace valvestate {

class Clipping
{
    public:
        void process(float *buffer, std::size_t buffer_size);
};

}
}
}
