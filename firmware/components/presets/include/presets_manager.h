#include "presets_api.h"

namespace shrapnel::presets {

/* Design notes:
 * 
 * Able to manage more presets than reasonable could fit into memory.
 * 
 * How do we iterate over the presets in the low level in esp-idf?
 * 
 * NVS iterators seem appropriate:
 * - nvs_entry_find to initialise an iterator. Filtered to a specific partition,
 *   namespace and type.
 * - nvs_entry_info to get the namespace, key and the type
 * 
 * If we put all presets in a single namespace, then we can iterate them easily.
 */
class PresetsManager {
    [[nodiscard]] int create() {
        
    }
    
    [[nodiscard]] int update() {
        
    }
    
    void remove() {
        
    }
    
    void for_each(void (*)(PresetData preset)) {

    }
};

}