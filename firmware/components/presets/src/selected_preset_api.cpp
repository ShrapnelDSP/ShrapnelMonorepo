#include "selected_preset_api.h"
#include "selected_preset.pb.h"

namespace shrapnel::api {

template <>
std::optional<shrapnel_selected_preset_Read>
to_proto(const selected_preset::Read &)
{
    shrapnel_selected_preset_Read out = shrapnel_selected_preset_Read_init_zero;
    return out;
}

template <>
std::optional<shrapnel_selected_preset_Notify>
to_proto(const selected_preset::Notify &message)
{
    shrapnel_selected_preset_Notify out = shrapnel_selected_preset_Notify_init_zero;
    out.id = message.selectedPresetId;
    return out;
}

template <>
std::optional<shrapnel_selected_preset_Write>
to_proto(const selected_preset ::Write &message)
{
    shrapnel_selected_preset_Write out = shrapnel_selected_preset_Write_init_zero;
    out.id = message.selectedPresetId;
    return out;
}

template <>
std::optional<shrapnel_selected_preset_Message>
to_proto(const selected_preset::SelectedPresetApiMessage &message)
{
    return std::visit(
        [](const auto &message)
            -> std::optional<shrapnel_selected_preset_Message>
        {
            shrapnel_selected_preset_Message out =
                shrapnel_selected_preset_Message_init_zero;

            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, selected_preset::Read>)
            {
                out.which_message = shrapnel_selected_preset_Message_read_tag;
                out.message.read =
                    *to_proto<shrapnel_selected_preset_Read>(message);
            }
            else if constexpr(std::is_same_v<T, selected_preset::Notify>)
            {
                out.which_message = shrapnel_selected_preset_Message_notify_tag;
                out.message.notify =
                    *to_proto<shrapnel_selected_preset_Notify>(message);
            }
            else if constexpr(std::is_same_v<T, selected_preset::Write>)
            {
                out.which_message = shrapnel_selected_preset_Message_write_tag;
                out.message.write =
                    *to_proto<shrapnel_selected_preset_Write>(message);
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
std::optional<selected_preset::Read>
from_proto(const shrapnel_selected_preset_Read &message)
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
