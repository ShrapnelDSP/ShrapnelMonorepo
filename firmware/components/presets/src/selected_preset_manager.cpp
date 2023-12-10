#include "selected_preset_manager.h"

namespace shrapnel::selected_preset {

constexpr char key[] = "selected_preset";

int SelectedPresetManager::set(presets::id_t id) { 
    int rc = storage->save(key, id);
    if(rc != 0)
    {
        return -1;
    }
    
    return 0;
}

int SelectedPresetManager::get(id_t &id) {
    int rc = storage->load(key, id);
    if(rc != 0)
    {
        return -1;
    }
    
    return 0;
}

}