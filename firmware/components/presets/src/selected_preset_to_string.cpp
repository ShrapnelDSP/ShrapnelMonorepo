#include "etl_utility.h"
#include "selected_preset_api.h"

namespace shrapnel::selected_preset {

etl::string_stream &operator<<(etl::string_stream &out, const Read &self)
{
    return out << "{ }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Notify &self)
{
    return out << "{ selected_preset=" << self.selectedPresetId << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Write &self)
{
    return out << "{ selected_preset=" << self.selectedPresetId << " }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const SelectedPresetApiMessage &self)
{
    auto print = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Read>)
        {
            out << "<Read>" << message;
        }
        else if constexpr(std::is_same_v<T, Notify>)
        {
            out << "<Notify>" << message;
        }
        else if constexpr(std::is_same_v<T, Write>)
        {
            out << "<Write>" << message;
        }
        else
        {
            out << "Unknown";
        }
    };

    std::visit(print, self);
    return out;
}

} // namespace shrapnel::selected_preset