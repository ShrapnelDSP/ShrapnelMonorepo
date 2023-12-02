#include "presets_api.h"
#include "presets_storage.h"

namespace shrapnel::presets {

class PresetsManager
{
public:
    [[nodiscard]] int create(const PresetData &preset)
    {
        
        
        storage.save();
        return 0;
    }

    [[nodiscard]] int update() {}

    void remove() {}

    void for_each(void (*)(PresetData preset)) {}

private:
    presets_storage::Storage storage;
};

} // namespace shrapnel::presets