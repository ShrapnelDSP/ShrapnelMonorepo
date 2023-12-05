#include "presets_api.h"
#include "presets_storage.h"
#include "preset_serialisation.h"

namespace shrapnel::presets {

class PresetsManager
{
public:
    [[nodiscard]] int create(const PresetData &preset)
    {
        std::array<uint8_t, 100> buffer;
        auto data = std::span<uint8_t, std::dynamic_extent>(buffer);
        
        serialise_preset();
       
        storage.save(preset.id, );
        return 0;
    }

    [[nodiscard]] int update() {}

    void remove() {}

    void for_each(void (*)(PresetData preset)) {}

private:
    presets_storage::Storage storage;
};

} // namespace shrapnel::presets