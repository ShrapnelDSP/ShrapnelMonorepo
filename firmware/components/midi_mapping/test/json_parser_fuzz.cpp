#include "midi_mapping_json_parser.h"
#include "etl/string.h"
#include "etl/string_stream.h"
#include <iostream>

int main(int argc, char* argv[])
{
    std::string line;
    while(std::getline(std::cin, line))
    {
        auto message = shrapnel::midi::MappingApiMessageBuilder::from_json(line.c_str());
        std::cout << message.index() << std::endl;
    }

    return 0;
}
