#include "presets_manager.h"

#define TAG "preset_manager"

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
    rc = storage->create(data, key_string);
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
    int rc = storage->load(id_to_key(id), data);
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

    rc = storage->save(id_to_key(id), data);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

void PresetsManager::remove(id_t id) { storage->remove(id_to_key(id)); }

void PresetsManager::for_each(
    etl::delegate<void(id_t, const PresetData &)> callback)
{
    for(const auto &entry : *storage)
    {
        auto id = key_to_id(entry.key);
        auto preset = read(id);
        if(!preset.has_value())
        {
            ESP_LOGE(TAG, "Failed to read preset at key %s", entry.key);
            continue;
        }
        callback(id, *preset);
    }
}

} // namespace 