if(NOT DEFINED TESTING)
idf_component_register(SRCS "src/audio_events.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES rapidjson
                       PRIV_REQUIRES compiler_warning_flags)
else()
    add_library(audio_events STATIC)
    add_library(shrapnel::audio_events ALIAS audio_events)

    target_sources(audio_events PRIVATE src/audio_events.cpp)

    target_include_directories(audio_events PUBLIC include)

    target_link_libraries(audio_events
            PUBLIC shrapnel::rapidjson
            PRIVATE shrapnel::compiler_warning_flags)
endif()
