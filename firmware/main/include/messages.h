#pragma once

#include <variant>
#include <optional>
#include <cmd_handling_api.h>
#include <midi_mapping_api.h>
#include <audio_events.h>

using ApiMessage = std::variant<shrapnel::parameters::ApiMessage,
    shrapnel::midi::MappingApiMessage,
    shrapnel::events::ApiMessage>;
using FileDescriptor = std::optional<int>;
using AppMessage = std::pair<ApiMessage, FileDescriptor>;
