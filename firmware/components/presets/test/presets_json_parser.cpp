/*
* Copyright 2022 Barabas Raffai
*
* This file is part of ShrapnelDSP.
*
* ShrapnelDSP is free software: you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option)
* any later version.
*
* ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along with
* ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
*/

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <bits/ranges_util.h>
#include <variant>

#include "presets_json_parser.h"

namespace {

using namespace shrapnel::presets;
using namespace shrapnel;

std::optional<PresetsApiMessage> parse_json(const char *json)
{
    rapidjson::Document document;
    document.Parse(json);

    assert(!document.HasParseError() && "Testing with invalid documents will "
                                        "not exercise the function under test");
    return from_json<PresetsApiMessage>(document);
}

TEST(PresetsJsonParser, Initialise)
{
    auto json = R"({
     "messageType": "Presets::initialise"
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Initialise>(*result), true);
}

TEST(PresetsJsonParser, Create)
{
    auto json = R"({
      "messageType": "Presets::create",
      "preset": {
        "name": "test_preset",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
      }
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Create>(*result), true);

    if(auto message = std::get_if<Create>(&(*result)))
    {
        Create expected{
            .preset{
                .name{"test_preset"},
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
        };

        EXPECT_THAT(*message, expected);
    }
    else
    {
        assert(false);
    }
}

TEST(PresetsJsonParser, CreateTruncatesLongName)
{
    auto json = R"({
      "messageType": "Presets::create",
      "preset": {
        "name": "1234567890123456789012345678901234567890",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
      }
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Create>(*result), true);

    if(auto message = std::get_if<Create>(&(*result)))
    {
        Create expected{
            .preset{
                .name{"12345678901234567890123456789012"},
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
        };

        EXPECT_THAT(*message, expected);
    }
    else
    {
        assert(false);
    }
}

TEST(PresetsJsonParser, CreateWithCorruptedProtoThrows)
{
    auto json = R"({
      "messageType": "Presets::create",
      "preset": {
        "name": "1234567890123456789012345678901234567890",
        "parameters": "bad"
      }
    })";

    // FIXME: production code is running with no exceptions, this should return
    // a std::nullopt instead.
    EXPECT_THROW(parse_json(json), std::exception);
}

TEST(PresetsJsonParser, Update)
{
    auto json = R"({
      "messageType": "Presets::update",
      "id": 42,
      "preset": {
        "name": "test_preset",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
      }
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Update>(*result), true);

    if(auto message = std::get_if<Update>(&(*result)))
    {
        Update expected{
            .id{42},
            .preset{
                .name{"test_preset"},
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
        };

        EXPECT_THAT(*message, expected);
    }
    else
    {
        assert(false);
    }
}

TEST(PresetsJsonParser, Delete)
{
    auto json = R"({"id":1,"messageType":"Presets::delete"})";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Delete>(*result), true);

    if(auto message = std::get_if<Delete>(&(*result)))
    {
        Delete expected{.id{1}};

        EXPECT_THAT(*message, expected);
    }
    else
    {
        assert(false);
    }
}

} // namespace
