#pragma once


#include "persistence.h"

namespace shrapnel::persistence {

class EspStorage final : Storage {
public:
    int save(const char *key, etl::string_view data) override;
    int load(const char *key, etl::istring &data) override;
};

}
