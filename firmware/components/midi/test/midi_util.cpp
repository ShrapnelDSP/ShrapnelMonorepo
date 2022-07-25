#include "midi_util.h"

namespace shrapnel {
namespace midi {

std::ostream& operator <<(std::ostream& out, const Message& message)
{
    etl::string<32> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    return out << buffer.data();
}

}
}
