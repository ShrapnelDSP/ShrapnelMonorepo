#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <bits/ranges_util.h>
#include <variant>

#include "midi_mapping_json_parser.h"

namespace {

using namespace shrapnel::midi;
using namespace shrapnel;

class MappingApiMessageTest : public ::testing::Test
{
    protected:

    MappingApiMessageTest() : sut() {}

    MappingApiMessageBuilder sut;
};


// Test plan
//
// One of each type of message decoded correctly
// std::variant used to encode the output
// Test does not depend on the JSON library, string goes in, std::variant comes
// out
TEST_F(MappingApiMessageTest, GetRequest)
{
    auto json = R"({
      "messageType": "MidiMap::get::request"
    })";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<GetRequest>(result), true);
}

TEST_F(MappingApiMessageTest, UnknownMessageType)
{
    auto json = R"({
      "messageType": "unknown"
    })";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<std::monostate>(result), true);
}

TEST_F(MappingApiMessageTest, NotStringMessageType)
{
    auto json = R"({
      "messageType": 42
    })";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<std::monostate>(result), true);
}

TEST_F(MappingApiMessageTest, MissingMessageType)
{
    auto json = R"({
        "hello": "world"
    })";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<std::monostate>(result), true);
}

TEST_F(MappingApiMessageTest, InvalidInput)
{
    auto json = R"(invalid)";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<std::monostate>(result), true);
}

TEST_F(MappingApiMessageTest, CreateRequest)
{
    auto json = R"({
      "messageType": "MidiMap::create::request",
      "mapping": {
        "00010203-0405-0607-0809-0a0b0c0d0e0f": {
          "midi_channel": 1,
          "cc_number": 2,
          "parameter_id": "gain"
        }
      }
    })";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<CreateRequest>(result), true);

    if (auto message = std::get_if<CreateRequest>(&result)) {
        auto expected = CreateRequest({
            Mapping::id_t{
                0, 1,  2,  3,  4,  5,  6,  7,
                8, 9, 10, 11, 12, 13, 14, 15
            },
            Mapping{1, 2, parameters::id_t("gain")}});

        // TODO need a custom matcher to get readable error output or at least
        //      a stringify operator for the classes used here
        EXPECT_THAT(*message, expected);
    } else {
        assert(false);
    }
#if 0
    CreateRequest({
            Mapping::id_t{
                0, 1,  2,  3,  4,  5,  6,  7,
                8, 9, 10, 11, 12, 13, 14, 15
            },
            Mapping{1, 2, parameters::id_t("gain")}});
#endif
}

// TODO a lot of the edge cases are not tested here. We need to test every
// field in every message under these conditions:
//
// - The field is missing
// - The field is present, but has the wrong type of value inside of it
// - The field is present, and contains a value of the correct type that is out
//   of range or invalid for the field

}
