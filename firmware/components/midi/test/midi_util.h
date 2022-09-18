#pragma once

#include <ostream>
#include "midi.h"

namespace shrapnel {
namespace midi {

std::ostream& operator <<(std::ostream& out, const Message& message);

}
}
