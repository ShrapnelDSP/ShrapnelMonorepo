#include "selected_preset_json_builder.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Disable warning inside rapidjson
// https://github.com/Tencent/rapidjson/issues/1700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#pragma GCC diagnostic pop

namespace {

using namespace shrapnel;
using namespace shrapnel::selected_preset;

template <typename T>
std::string write_json(const T &object)
{
    rapidjson::Document document;
    auto result = to_json(document, object);

    result.Swap(document);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

std::string normalise_json(const std::string &json)
{
    rapidjson::Document document;
    document.Parse(json.c_str());

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

TEST(SelectedPresetJsonBuilder, Notify)
{
    SelectedPresetApiMessage input{
        Notify{
            .selectedPresetId{42},
        },
    };

    rapidjson::Document document;

    auto reference = normalise_json(R"({
          "selectedPreset": 42,
          "messageType": "SelectedPreset::notify"
        })");

    EXPECT_THAT(write_json(input), reference);
}

} // namespace