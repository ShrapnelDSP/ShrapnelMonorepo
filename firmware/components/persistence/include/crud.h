#pragma once

#include <cstdint>
#include <etl/delegate.h>
#include <optional>

namespace shrapnel::persistence {

template <typename DataT>
class Crud
{
public:
    [[nodiscard]] virtual int create(const DataT &data, uint32_t &id_out) = 0;
    [[nodiscard]] virtual int read(uint32_t id, DataT &data_out) = 0;
    [[nodiscard]] virtual int update(uint32_t id, const DataT &data) = 0;
    [[nodiscard]] virtual int destroy(uint32_t id) = 0;

    virtual void
    for_each(etl::delegate<void(uint32_t, const DataT &)> callback) = 0;
};

} // namespace shrapnel::persistence
