#pragma once


#include "persistence.h"

namespace shrapnel::persistence {

/** Persistent storage implemented using ESP32 nvs component
 *
 * \note Key will be truncated to 15 characters due to limitation of NVS
 */
class EspStorage final : public Storage {
public:
    int save(const char *key, etl::string_view data) override;
    int load(const char *key, etl::istring &data) override;
};

}
