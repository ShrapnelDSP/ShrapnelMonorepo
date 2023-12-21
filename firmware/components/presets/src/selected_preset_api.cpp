#include "selected_preset_api.h"
#include "selected_preset.pb.h"

namespace shrapnel::api {

template <>
int to_proto(const selected_preset::Read &, shrapnel_selected_preset_Read &)
{
    return 0;
}

template <>
int to_proto(const selected_preset::Notify &message,
             shrapnel_selected_preset_Notify &out)
{
    out.id = message.selectedPresetId;
    return 0;
}

template <>
int to_proto(const selected_preset ::Write &message,
             shrapnel_selected_preset_Write &out)
{
    out.id = message.selectedPresetId;
    return 0;
}

template <>
int to_proto(const selected_preset::SelectedPresetApiMessage &message,
             shrapnel_selected_preset_Message &out)
{
    return std::visit(
        [&out](const auto &message) -> int
        {
            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, selected_preset::Read>)
            {
                out.which_message = shrapnel_selected_preset_Message_read_tag;
                int rc = to_proto<shrapnel_selected_preset_Read>(
                    message, out.message.read);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, selected_preset::Notify>)
            {
                out.which_message = shrapnel_selected_preset_Message_notify_tag;
                int rc = to_proto<shrapnel_selected_preset_Notify>(
                    message, out.message.notify);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, selected_preset::Write>)
            {
                out.which_message = shrapnel_selected_preset_Message_write_tag;
                int rc = to_proto<shrapnel_selected_preset_Write>(
                    message, out.message.write);
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
std::optional<selected_preset::Read>
from_proto(const shrapnel_selected_preset_Read &)
{
    return {{}};
}

template <>
std::optional<selected_preset::Notify>
from_proto(const shrapnel_selected_preset_Notify &message)
{
    return {{.selectedPresetId{message.id}}};
}

template <>
std::optional<selected_preset::Write>
from_proto(const shrapnel_selected_preset_Write &message)
{
    return {{.selectedPresetId{message.id}}};
}

template <>
std::optional<selected_preset::SelectedPresetApiMessage>
from_proto(const shrapnel_selected_preset_Message &message)
{
    switch(message.which_message)
    {
    case shrapnel_selected_preset_Message_read_tag:
        return from_proto<selected_preset::Read>(message.message.read);
    case shrapnel_selected_preset_Message_notify_tag:
        return from_proto<selected_preset::Notify>(message.message.notify);
    case shrapnel_selected_preset_Message_write_tag:
        return from_proto<selected_preset::Write>(message.message.write);
    }

    return std::nullopt;
}

} // namespace shrapnel::api
