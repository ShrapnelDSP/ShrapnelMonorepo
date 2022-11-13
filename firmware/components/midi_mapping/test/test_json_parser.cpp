#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <bits/ranges_util.h>
#include <variant>

#include "midi_mapping_json_parser.h"

namespace shrapnel {
namespace midi {

std::ostream& operator <<(std::ostream& out, const CreateRequest& message)
{
    etl::string<128> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    return out << buffer.data();
}

}
}

namespace {

using namespace shrapnel::midi;
using namespace shrapnel;

std::optional<MappingApiMessage> parse_json(const char *json)
{
    rapidjson::Document document;
    document.Parse(json);

    assert(!document.HasParseError() && "Testing with invalid documents will "
            "not excercise the function under test");
    return from_json<MappingApiMessage>(document);
}


#if 0
std::ostream& operator <<(std::ostream& out, const CreateRequest& message)
{
    etl::string<32> buffer;
    etl::string_stream stream{buffer};
    stream << message;

    return out << buffer.data();
}
#endif

class MappingApiMessageTest : public ::testing::Test
{
    protected:

    MappingApiMessageTest() {}
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

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<GetRequest>(*result), true);
}

TEST_F(MappingApiMessageTest, UnknownMessageType)
{
    auto json = R"({
      "messageType": "unknown"
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), false);
}

TEST_F(MappingApiMessageTest, NotStringMessageType)
{
    auto json = R"({
      "messageType": 42
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), false);
}

TEST_F(MappingApiMessageTest, MissingMessageType)
{
    auto json = R"({
        "hello": "world"
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), false);
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

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<CreateRequest>(*result), true);

    if (auto message = std::get_if<CreateRequest>(&(*result))) {
        auto expected = CreateRequest({
            Mapping::id_t{
                0, 1,  2,  3,  4,  5,  6,  7,
                8, 9, 10, 11, 12, 13, 14, 15
            },
            Mapping{1, 2, parameters::id_t("gain")}});

        EXPECT_THAT(*message, expected);
    } else {
        assert(false);
    }
}

TEST_F(MappingApiMessageTest, Update)
{
    auto json = R"({
      "messageType": "MidiMap::update",
      "mapping": {
        "00010203-0405-0607-0809-0a0b0c0d0e0f": {
          "midi_channel": 1,
          "cc_number": 2,
          "parameter_id": "gain"
        }
      }
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Update>(*result), true);

    if (auto message = std::get_if<Update>(&(*result))) {
        auto expected = Update({
            Mapping::id_t{
                0, 1,  2,  3,  4,  5,  6,  7,
                8, 9, 10, 11, 12, 13, 14, 15
            },
            Mapping{1, 2, parameters::id_t("gain")}});

        EXPECT_THAT(*message, expected);
    } else {
        assert(false);
    }
}

TEST_F(MappingApiMessageTest, Remove)
{
    auto json = R"({
      "messageType": "MidiMap::remove",
      "id": "00010203-0405-0607-0809-0a0b0c0d0e0f"
    })";

    auto result = parse_json(json);
    EXPECT_THAT(result.has_value(), true);
    EXPECT_THAT(std::holds_alternative<Remove>(*result), true);

    if (auto message = std::get_if<Remove>(&(*result))) {
        auto expected = Remove(Mapping::id_t{
                0, 1,  2,  3,  4,  5,  6,  7,
                8, 9, 10, 11, 12, 13, 14, 15
            });

        EXPECT_THAT(*message, expected);
    } else {
        assert(false);
    }
}

// TODO a lot of the edge cases are not tested here. We need to test every
// field in every message under these conditions:
//
// - The field is missing
// - The field is present, but has the wrong type of value inside of it
// - The field is present, and contains a value of the correct type that is out
//   of range or invalid for the field

}
