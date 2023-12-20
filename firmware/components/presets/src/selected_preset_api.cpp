#include "selected_preset_api.h"
#include "selected_preset.pb.h"

namespace shrapnel::api {

template <>
std::optional<shrapnel_selected_preset_Message>
to_proto(const selected_preset::SelectedPresetApiMessage &message)
{
    shrapnel_selected_preset_Message out =
        shrapnel_selected_preset_Message_init_zero;

    

    return out;
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

template <>
std::optional<std::span<uint8_t>>
to_bytes(const selected_preset::SelectedPresetApiMessage &message,
         std::span<uint8_t> buffer)
{
    auto proto = to_proto<shrapnel_selected_preset_Message>(message);
    return to_bytes(*proto, buffer);
}

template <>
std::optional<selected_preset::SelectedPresetApiMessage>
from_bytes(std::span<const uint8_t> buffer)
{
    auto proto = from_bytes<shrapnel_selected_preset_Message>(buffer);
    return from_proto<selected_preset::SelectedPresetApiMessage>(*proto);
}

} // namespace shrapnel::api
