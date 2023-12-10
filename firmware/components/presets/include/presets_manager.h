#include "preset_serialisation.h"
#include "presets_api.h"
#include "presets_storage.h"
#include <etl/delegate.h>
#include <memory>

namespace shrapnel::presets {

class PresetsManager
{
public:
    [[nodiscard]] int create(const PresetData &preset, id_t &id_out);
    [[nodiscard]] std::optional<PresetData> read(id_t id);
    [[nodiscard]] int update(id_t id, const PresetData &preset);
    void remove(id_t id);

    void for_each(etl::delegate<void(id_t, const PresetData &)> callback);

private:
    std::unique_ptr<presets_storage::Storage> storage;
};

} // namespace shrapnel::presets