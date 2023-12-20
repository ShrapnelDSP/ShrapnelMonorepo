#include "cmd_handling_api.h"
#include "cmd_handling.pb.h"

namespace shrapnel::api {

template <>
std::optional<shrapnel_cmd_handling_Update>
to_proto(const parameters::Update &message)
{
    shrapnel_cmd_handling_Update out = shrapnel_cmd_handling_Update_init_zero;
    out.value = message.value;
    strncpy(out.id, message.id.data(), sizeof out.id);
    return out;
}

template <>
std::optional<shrapnel_cmd_handling_Initialise>
to_proto(const parameters::Initialise &)
{
    return {{}};
}

template <>
std::optional<shrapnel_cmd_handling_Message>
to_proto(const parameters::ApiMessage &message)
{
    return std::visit(
        [](const auto &message) -> std::optional<shrapnel_cmd_handling_Message>
        {
            shrapnel_cmd_handling_Message out =
                shrapnel_cmd_handling_Message_init_zero;

            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, parameters::Update>)
            {
                out.which_message = shrapnel_cmd_handling_Message_update_tag;
                out.message.update =
                    *to_proto<shrapnel_cmd_handling_Update>(message);
            }
            else if constexpr(std::is_same_v<T, parameters::Initialise>)
            {
                out.which_message =
                    shrapnel_cmd_handling_Message_initialise_tag;
                out.message.initialise =
                    *to_proto<shrapnel_cmd_handling_Initialise>(message);
            }
            else
            {
                return std::nullopt;
            }

            return out;
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