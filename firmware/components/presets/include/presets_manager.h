#include "preset_serialisation.h"
#include "presets_api.h"
#include "presets_storage.h"

namespace shrapnel::presets {

class PresetsManager
{
public:
    [[nodiscard]] int create(const PresetData &preset, id_t &id_out);
    [[nodiscard]] std::optional<PresetData> read(id_t id);
    [[nodiscard]] int update(id_t id, const PresetData &preset);
    void remove(id_t id);

private:
    static inline etl::string<15> id_to_key(id_t id)
    {
        char hex[9];
        int rc = snprintf(hex, sizeof hex, "%08" PRIx32, id);
        assert(rc == 9);
        return etl::string<15>(hex, 9);
    };

    static inline id_t key_to_id(const etl::string<15> &key)
    {
        id_t id;
        int rc = sscanf(key.c_str(), "%08" PRIx32, &id);
        assert(rc == 1);
        return id;
    };

    presets_storage::Storage storage;
};

} // namespace shrapnel::presets