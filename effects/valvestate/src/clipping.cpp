
#include "clipping.h"
#include "wave_shape.h"

namespace shrapnel {
namespace effect {
namespace valvestate {

void Clipping::process(float *buffer, std::size_t buffer_size)
{
    for(std::size_t i = 0; i < buffer_size; i++)
    {
        buffer[i] = waveshape(buffer[i]);
    }
}

}
}
}
