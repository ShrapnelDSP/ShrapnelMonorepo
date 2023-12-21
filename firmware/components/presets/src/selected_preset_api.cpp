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
int from_proto(const shrapnel_selected_preset_Read &, selected_preset::Read &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_selected_preset_Notify &message,
               selected_preset::Notify &out)
{
    out.selectedPresetId = message.id;
    return 0;
}

template <>
int from_proto(const shrapnel_selected_preset_Write &message,
               selected_preset::Write &out)
{
    out.selectedPresetId = message.id;
    return 0;
}

template <>
int from_proto(const shrapnel_selected_preset_Message &message,
               selected_preset::SelectedPresetApiMessage &out)
{
    switch(message.which_message)
    {
    case shrapnel_selected_preset_Message_read_tag:
    {
        selected_preset::Read tmp{};
        from_proto<selected_preset::Read>(message.message.read, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_selected_preset_Message_notify_tag:
    {
        selected_preset::Notify tmp{};
        from_proto<selected_preset::Notify>(message.message.notify, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_selected_preset_Message_write_tag:
    {
        selected_preset::Write tmp{};
        from_proto<selected_preset::Write>(message.message.write, tmp);
        out = tmp;
        return 0;
    }
    }

    return -1;
}

} // namespace shrapnel::api
