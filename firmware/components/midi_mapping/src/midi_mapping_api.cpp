#include "midi_mapping_api.h"
#include "etl_utility.h"


namespace shrapnel {
namespace midi {

etl::string_stream& operator<<(etl::string_stream&  out, const GetRequest& self) {
    out << "{}";
    return out;
}

etl::string_stream& operator<<(etl::string_stream&  out, const GetResponse& self)
{
    return out << "{ TODO }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const CreateRequest& self) {
    out << "{ " << self.mapping << " }";
    return out;
}

etl::string_stream& operator<<(etl::string_stream&  out, const CreateResponse& self)
{
    return out << "{ " << self.mapping << " }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const Update& self)
{
    return out << "{ " << self.mapping << " }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const Remove& self)
{
    return out << "{ " << self.id << " }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const MappingApiMessage& self)
{
#if 1
    auto print = [&](const auto &message) {
        using T = std::decay_t<decltype(message)>;

        // TODO can we just replace this with out << message? The type is
        //      already statically known because of auto
        if constexpr (std::is_same_v<T, GetRequest>) {
            out << message;
        } else if constexpr (std::is_same_v<T, GetResponse>) {
            out << message;
        } else if constexpr (std::is_same_v<T, CreateRequest>) {
            out << message;
        } else if constexpr (std::is_same_v<T, CreateResponse>) {
            out << message;
        } else if constexpr (std::is_same_v<T, Update>) {
            out << message;
        } else if constexpr (std::is_same_v<T, Remove>) {
            out << message;
        } else {
            out << "Unknown";
        }
    };

    std::visit(print, self);

#else

    if constexpr (auto message = std::get_if<GetRequest>(&self)) {
        out << message;
    } else if constexpr (auto message = std::get_if<GetResponse>(&self)) {
        out << message;
    } else if constexpr (auto message = std::get_if<CreateRequest>(&self)) {
        out << message;
    } else if constexpr (auto message = std::get_if<CreateResponse>(&self)) {
        out << message;
    } else if constexpr (auto message = std::get_if<Update>(&self)) {
        out << message;
    } else if constexpr (auto message = std::get_if<Remove>(&self)) {
        out << message;
    } else {
        out << "Unknown";
    }

#endif

    return out;
}

}
}
