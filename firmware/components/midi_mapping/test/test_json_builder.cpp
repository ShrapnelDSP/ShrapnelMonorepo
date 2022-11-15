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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <variant>

#include "midi_mapping_api.h"
#include "midi_mapping_json_builder.h"
#include "midi_mapping_json_parser.h"
#include "rapidjson/document.h"

namespace {

using namespace shrapnel;
using namespace shrapnel::midi;

template<typename T>
std::string write_json(const T &object)
{
    rapidjson::Document document;
    auto result = to_json(document, object);

    result.Swap(document);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

std::string normalise_json(const std::string &json)
{
    rapidjson::Document document;
    document.Parse(json.c_str());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

TEST(MappingJsonBuilder, Mapping)
{
    Mapping input{1, 2, "test"};

    rapidjson::Document document;
    auto roundtrip = from_json<Mapping>(to_json(document, input));
    EXPECT_THAT(roundtrip, input);
}

TEST(MappingJsonBuilder, CreateResponse)
{
    CreateResponse input{{
        Mapping::id_t{
            0,  1,  2,  3,  4,  5,  6,  7,
            8,  9, 10, 11, 12, 13, 14, 15,
        },
        Mapping(1, 2, "test")
    }};

    auto reference = normalise_json(R"({
          "mapping": {
            "00010203-0405-0607-0809-0a0b0c0d0e0f": {
              "midi_channel": 1,
              "cc_number": 2,
              "parameter_id": "test"
            }
          }
        })");

    EXPECT_THAT(write_json(input), reference);
}

TEST(MappingJsonBuilder, DISABLED_VariantCreateResponse)
{
    MappingApiMessage input{CreateResponse({
        Mapping::id_t{
            0,  1,  2,  3,  4,  5,  6,  7,
            8,  9, 10, 11, 12, 13, 14, 15,
        },
        Mapping(1, 2, "test")
    })};

    auto reference = normalise_json(R"({
          "messageType": "MidiMap::create::response",
          "mapping": {
            "00010203-0405-0607-0809-0a0b0c0d0e0f": {
              "midi_channel": 1,
              "cc_number": 2,
              "parameter_id": "test"
            }
          }
        })");

    // TODO this creates a false negative, because the order of the fields is
    // different. There doesn't seem to be a way to sort the fields in
    // rapidjson.
    //
    // We might just have to manually create golden style JSON strings to test
    // against.
    //
    // We can instead compare the DOM, but not sure if equality is implemented
    // for it. It also produces bad output from gtest, it just prints the Value
    // as bytes.
    EXPECT_THAT(write_json(input), reference);
}

}
