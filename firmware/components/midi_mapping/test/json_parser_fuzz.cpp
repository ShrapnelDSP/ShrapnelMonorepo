#include "midi_mapping_json_parser.h"
#include "etl/string.h"
#include "etl/string_stream.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::string json;
    std::getline(std::cin, json, '\0');
    auto message = shrapnel::midi::MappingApiMessageBuilder::from_json(json.c_str());
    std::cout << message.index() << std::endl;
    return 0;
}
