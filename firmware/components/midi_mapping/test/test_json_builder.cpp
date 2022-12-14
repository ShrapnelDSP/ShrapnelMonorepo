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

#include "audio_param.h"
#include "midi_mapping.h"
#include "midi_mapping_api.h"
#include "midi_mapping_json_builder.h"
#include "midi_mapping_json_parser.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

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

TEST(MappingJsonBuilder, Mapping)
{
    Mapping input{1, 2, Mapping::Mode::PARAMETER, "test"};

    rapidjson::Document document;
    auto roundtrip = from_json<Mapping>(to_json(document, input));
    EXPECT_THAT(roundtrip, input);
}

TEST(MappingJsonBuilder, CreateResponse)
{
    CreateResponse input{{Mapping::id_t{
                              0,
                              1,
                              2,
                              3,
                              4,
                              5,
                              6,
                              7,
                              8,
                              9,
                              10,
                              11,
                              12,
                              13,
                              14,
                              15,
                          },
                          Mapping{1, 2, Mapping::Mode::PARAMETER, "test"}}};

    auto reference = normalise_json(R"({
          "mapping": {
            "00010203-0405-0607-0809-0a0b0c0d0e0f": {
              "midi_channel": 1,
              "cc_number": 2,
              "mode": "parameter",
              "parameter_id": "test"
            }
          }
        })");

    EXPECT_THAT(write_json(input), reference);
}

TEST(MappingJsonBuilder, VariantCreateResponse)
{
    MappingApiMessage input{
        CreateResponse{{Mapping::id_t{
                            0,
                            1,
                            2,
                            3,
                            4,
                            5,
                            6,
                            7,
                            8,
                            9,
                            10,
                            11,
                            12,
                            13,
                            14,
                            15,
                        },
                        Mapping{1, 2, Mapping::Mode::PARAMETER, "test"}}}};

    auto reference = normalise_json(R"({
          "mapping": {
            "00010203-0405-0607-0809-0a0b0c0d0e0f": {
              "midi_channel": 1,
              "cc_number": 2,
              "mode": "parameter",
              "parameter_id": "test"
            }
          },
          "messageType": "MidiMap::create::response"
        })");

    EXPECT_THAT(write_json(input), reference);
}

TEST(MappingJsonBuilder, VariantGetResponse)
{
    etl::map<Mapping::id_t, Mapping, 2> mapping{
        {Mapping::id_t{0}, Mapping{1, 2, Mapping::Mode::PARAMETER, "foo"}},
        {Mapping::id_t{1}, Mapping{3, 4, Mapping::Mode::TOGGLE, "bar"}}};

    MappingApiMessage input{
        GetResponse{&mapping}
    };

    auto reference = normalise_json(R"({
          "mappings": {
            "00000000-0000-0000-0000-000000000000": {
              "midi_channel": 1,
              "cc_number": 2,
              "mode": "parameter",
              "parameter_id": "foo"
            },
            "01000000-0000-0000-0000-000000000000": {
              "midi_channel": 3,
              "cc_number": 4,
              "mode": "toggle",
              "parameter_id": "bar"
            }
          },
          "messageType": "MidiMap::get::response"
        })");

    EXPECT_THAT(write_json(input), reference);
}

}
