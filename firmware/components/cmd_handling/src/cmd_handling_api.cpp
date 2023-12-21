#include "cmd_handling_api.h"
#include "cmd_handling.pb.h"

namespace shrapnel::api {

template <>
int to_proto(const parameters::Update &message,
             shrapnel_cmd_handling_Update &out)
{
    out.value = message.value;
    strncpy(out.id, message.id.data(), sizeof out.id);
    return 0;
}

template <>
int to_proto(const parameters::Initialise &, shrapnel_cmd_handling_Initialise &)
{
    return 0;
}

template <>
int to_proto(const parameters::ApiMessage &message,
             shrapnel_cmd_handling_Message &out)
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
                return to_proto<shrapnel_cmd_handling_Update>(
                    message, out.message.update);
            }
            else if constexpr(std::is_same_v<T, parameters::Initialise>)
            {
                out.which_message =
                    shrapnel_cmd_handling_Message_initialise_tag;
                return to_proto<shrapnel_cmd_handling_Initialise>(
                    message, out.message.initialise);
            }
            return -1;
        },
        message);
}

template <>
int from_proto(const shrapnel_cmd_handling_Update &message,
               parameters::Update &out)
{
    out.id = message.id;
    out.value = message.value;
    return 0;
}

template <>
int from_proto(const shrapnel_cmd_handling_Initialise &,
               parameters::Initialise &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_cmd_handling_Message &message,
               parameters::ApiMessage &out)
{
    switch(message.which_message)
    {
    case shrapnel_cmd_handling_Message_update_tag:
    {
        parameters::Update tmp{};
        int rc = from_proto<parameters::Update>(message.message.update, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_cmd_handling_Message_initialise_tag:
    {
        parameters::Initialise tmp{};
        int rc =
            from_proto<parameters::Initialise>(message.message.initialise, tmp);
        out = tmp;
        return rc;
    }
    }

    return -1;
}

}; // namespace shrapnel::api