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

#include "selected_preset_json_parser.h"

namespace {

using namespace shrapnel::selected_preset;
using namespace shrapnel;

std::optional<SelectedPresetApiMessage> parse_json(const char *json)
{
    rapidjson::Document document;
    document.Parse(json);

    assert(!document.HasParseError() && "Testing with invalid documents will "
                                        "not exercise the function under test");
    return from_json<SelectedPresetApiMessage>(document);
}

TEST(SelectedPresetJsonParser, Read)
{
    auto json = R"({
     "messageType": "SelectedPreset::read"
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Read>(*result), true);
}

TEST(SelectedPresetJsonParser, Write)
{
    auto json = R"({
      "messageType": "SelectedPreset::write",
      "selectedPreset": "00010203-0405-0607-0809-0a0b0c0d0e0f"
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Write>(*result), true);

    if(auto message = std::get_if<Write>(&(*result)))
    {
        Write expected{
            .selectedPresetId{
                presets::id_t{
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            },
        };

        EXPECT_THAT(*message, expected);
    }
    else
    {
        assert(false);
    }
}

} // namespace
