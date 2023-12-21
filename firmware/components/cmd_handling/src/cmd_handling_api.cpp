#include "cmd_handling_api.h"
#include "cmd_handling.pb.h"

namespace shrapnel::api {

template <>
int
to_proto(const parameters::Update &message, shrapnel_cmd_handling_Update &out)
{
    out.value = message.value;
    strncpy(out.id, message.id.data(), sizeof out.id);
    return 0;
}

template <>
int
to_proto(const parameters::Initialise &, shrapnel_cmd_handling_Initialise &)
{
    return 0;
}

template <>
int
to_proto(const parameters::ApiMessage &message, shrapnel_cmd_handling_Message &out)
{
    return std::visit(
        [](const auto &message) -> int 
        {
            shrapnel_cmd_handling_Message out =
                shrapnel_cmd_handling_Message_init_zero;

            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, parameters::Update>)
            {
                out.which_message = shrapnel_cmd_handling_Message_update_tag;
                int rc = to_proto<shrapnel_cmd_handling_Update>(message, out.message.update);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, parameters::Initialise>)
            {
                out.which_message =
                    shrapnel_cmd_handling_Message_initialise_tag;
                int rc = to_proto<shrapnel_cmd_handling_Initialise>(message, out.message.initialise);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }

            return 0;
        },
        message);
}

template <>
std::optional<parameters::Update>
from_proto(const shrapnel_cmd_handling_Update &message)
{
    return {
        {
            .id{message.id},
            .value{message.value},
        },
    };
}

template <>
std::optional<parameters::Initialise>
from_proto(const shrapnel_cmd_handling_Initialise &)
{
    return {{}};
}

template <>
std::optional<parameters::ApiMessage>
from_proto(const shrapnel_cmd_handling_Message &message)
{
    switch(message.which_message)
    {
    case shrapnel_cmd_handling_Message_update_tag:
        return from_proto<parameters::Update>(message.message.update);
    case shrapnel_cmd_handling_Message_initialise_tag:
        return from_proto<parameters::Initialise>(message.message.initialise);
    }

    return std::nullopt;
}

}; // namespace shrapnel::api