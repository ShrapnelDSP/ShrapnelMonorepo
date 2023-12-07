#include "preset_serialisation.h"
#include "presets_api.h"
#include "presets_storage.h"

namespace shrapnel::presets {

class PresetsManager
{
public:
    [[nodiscard]] int create(const PresetData &preset)
    {
        std::array<uint8_t, 100> buffer;
        auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

        int rc = serialise_preset(preset, data);
        if(rc != 0)
        {
            return -1;
        }

        storage.save(id_to_key(preset.id), data);
        return 0;
    }
    
    [[nodiscard]] PresetData read(const id_t id) {
        std::array<uint8_t, 100> buffer;
        auto data = std::span<uint8_t, std::dynamic_extent>(buffer);
        storage.load(id_to_key(id), data);
    }

    [[nodiscard]] int update(const PresetData &preset) {
        return create(preset);
    }

    void remove(const id_t &id) {
        storage.remove(id_to_key(id));
    }

private:
    static etl::string<15> id_to_key(const id_t &id) {
        // FIXME how do we convert a UUID to a 15 character string?
        // can't just take the UUID, key is a null terminated string, can't
        // have 0 in the middle
    };
    
    presets_storage::Storage storage;
};

} // namespace shrapnel::presets