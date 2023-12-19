#include "cmd_handling_api.h"

namespace shrapnel::api {

std::optional<std::span<uint8_t>>
to_bytes(const shrapnel::parameters::ApiMessage &message,
                        std::span<uint8_t> buffer)
{
    return {};
}

std::optional<parameters::ApiMessage>
from_bytes(std::span<const uint8_t> buffer)
{
    return {};
}

}; // namespace shrapnel::api