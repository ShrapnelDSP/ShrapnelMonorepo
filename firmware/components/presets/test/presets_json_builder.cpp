#include "presets_json_builder.h"

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
using namespace shrapnel::presets;

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

TEST(PresetsJsonBuilder, Notify)
{
    PresetsApiMessage input{
        Notify{
            .id{42},
            .preset{
                .name{"test"},
                .parameters{
                    .amp_gain{0.001},
                    .amp_channel{0.002},
                    .bass{0.003},
                    .middle{0.004},
                    .treble{0.005},
                    .contour{0.006},
                    .volume{0.007},
                    .noise_gate_threshold{0.008},
                    .noise_gate_hysteresis{0.009},
                    .noise_gate_attack{0.010},
                    .noise_gate_hold{0.011},
                    .noise_gate_release{0.012},
                    .noise_gate_bypass{0.013},
                    .chorus_rate{0.014},
                    .chorus_depth{0.015},
                    .chorus_mix{0.016},
                    .chorus_bypass{0.017},
                    .wah_position{0.018},
                    .wah_vocal{0.019},
                    .wah_bypass{0.020},
                },
            },
        },
    };

    rapidjson::Document document;

    auto reference = normalise_json(R"({
          "id": 42,
          "preset": {
            "name": "test",
            "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
          },
          "messageType": "Presets::notify"
        })");

    EXPECT_THAT(write_json(input), reference);
}

} // namespace
