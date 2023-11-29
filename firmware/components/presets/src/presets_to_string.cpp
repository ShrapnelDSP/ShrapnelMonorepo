#include "etl_utility.h"
#include "presets_api.h"

namespace shrapnel::presets {

etl::string_stream &operator<<(etl::string_stream &out,
                               const ParametersData &self)
{
    return out << "{ amp_gain=" << self.amp_gain << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const PresetData &self)
{
    return out << "{ id=" << self.id << " name=" << self.name
               << "parameters=" << self.parameters << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Initialise &self)
{
    return out << "{ }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Notify &self)
{
    return out << "{ preset=" << self.preset << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Create &self)
{
    return out << "{ preset=" << self.preset << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Update &self)
{
    return out << "{ preset=" << self.preset << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Delete &self)
{
    return out << "{ id=" << self.id << " }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const PresetsApiMessage &self)
{
    auto print = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Initialise>)
        {
            out << "<Initialise>" << message;
        }
        else if constexpr(std::is_same_v<T, Notify>)
        {
            out << "<Notify>" << message;
        }
        else if constexpr(std::is_same_v<T, Create>)
        {
            out << "<Create>" << message;
        }
        else if constexpr(std::is_same_v<T, Update>)
        {
            out << "<Update>" << message;
        }
        else if constexpr(std::is_same_v<T, Delete>)
        {
            out << "<Delete>" << message;
        }
        else
        {
            out << "Unknown";
        }
    };

    std::visit(print, self);
    return out;
}

} // namespace shrapnel::presets