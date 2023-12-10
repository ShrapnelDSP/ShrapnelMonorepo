#pragma once

#include "persistence.h"
#include "presets.h"

namespace shrapnel::selected_preset {

class SelectedPresetManager
{
public:
    explicit SelectedPresetManager(std::shared_ptr<persistence::Storage> storage);
    
    [[nodiscard]] int set(presets::id_t id);
    [[nodiscard]] int get(presets::id_t &id);

private:
    std::shared_ptr<persistence::Storage> storage;
};

} // namespace shrapnel::selected_preset