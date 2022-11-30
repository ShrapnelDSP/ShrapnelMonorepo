#pragma once

#include "etl/string.h"
#include "etl/string_view.h"

namespace shrapnel::persistence {

/** Interface for storing persistent data
 *
 * The data sent to \ref save should be reloaded by \ref load even after power
 * down.
 */
class Storage {
public:
    /// \return  non-zero on error
    virtual int save(const char *key, etl::string_view data) = 0;
    /// \return  non-zero on error
    virtual int load(const char *key, etl::istring &data) = 0;
};

}