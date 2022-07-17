#pragma once

#include "gmock/gmock.h"
#include "midi.h"

using testing::AllOf;
using testing::Field;
using testing::ExplainMatchResult;

using namespace shrapnel::midi;

MATCHER_P(MessageMatches, other, "")
{
    return ExplainMatchResult(
            AllOf(Field("type", &Message::type, other.type),
                  Field("note on", &Message::note_on, Field("note", &Message::NoteOnOff::note, other.note_on.note)),
                  Field("note on", &Message::note_on, Field("velocity", &Message::NoteOnOff::velocity, other.note_on.velocity))),
                  arg, result_listener);

}
