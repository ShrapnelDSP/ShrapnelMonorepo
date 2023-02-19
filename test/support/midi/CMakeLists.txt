add_library(midi_support STATIC)
add_library(shrapnel::midi_support ALIAS midi_support)

target_include_directories(midi_support PUBLIC midifile/include)

target_sources(midi_support
        PRIVATE
        midifile/src/Options.cpp
        midifile/src/Binasc.cpp
        midifile/src/MidiEvent.cpp
        midifile/src/MidiEventList.cpp
        midifile/src/MidiFile.cpp
        midifile/src/MidiMessage.cpp)

