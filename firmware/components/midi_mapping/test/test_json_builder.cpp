#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <variant>

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

TEST(MappingJsonBuilder, Mapping)
{
    Mapping input{1, 2, "test"};

    rapidjson::Document document;
    auto roundtrip = from_json<Mapping>(to_json(document, input));
    EXPECT_THAT(roundtrip, input);
}

}
