#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <variant>

#include "midi_mapping_json_parser.h"

namespace {

using namespace shrapnel::midi;

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
          "midi_channel": 0,
          "cc_number": 1,
          "parameter_id": "gain"
        }
      }
    })";

    auto result = sut.from_json(json);

    EXPECT_THAT(std::holds_alternative<std::monostate>(result), true);
}

}
