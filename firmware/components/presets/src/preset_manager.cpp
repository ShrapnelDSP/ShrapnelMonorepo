#include "presets_manager.h"

namespace shrapnel::presets {

int PresetsManager::create(const PresetData &preset, id_t &id_out)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    int rc = serialise_preset(preset, data);
    if(rc != 0)
    {
        return -1;
    }

    etl::string<15> key_string;
    rc = storage.create(data, key_string);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

std::optional<PresetData> PresetsManager::read(id_t id)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);
    int rc = storage.load(id_to_key(id), data);
    if(rc != 0)
    {
        return std::nullopt;
    }

    // FIXME: deserialise
    assert(false);
}

int PresetsManager::update(id_t id, const PresetData &preset)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    int rc = serialise_preset(preset, data);
    if(rc != 0)
    {
        return -1;
    }

    rc = storage.save(id_to_key(id), data);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

void PresetsManager::remove(id_t id) { storage.remove(id_to_key(id)); }

} // namespace shrapnel::presets