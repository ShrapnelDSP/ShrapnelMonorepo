#pragma once

#include "gmock/gmock.h"
#include "midi.h"

using testing::AllOf;
using testing::Field;
using testing::ExplainMatchResult;

using namespace shrapnel::midi;

MATCHER_P(NoteOnMatches, other, "")
{
    return ExplainMatchResult(
            AllOf(Field("note on", &Message::note_on, Field("note", &Message::NoteOnOff::note, other.note_on.note)),
                  Field("note on", &Message::note_on, Field("velocity", &Message::NoteOnOff::velocity, other.note_on.velocity))),
                  arg, result_listener);
}

MATCHER_P(NoteOffMatches, other, "")
{
    return ExplainMatchResult(
            AllOf(Field("note off", &Message::note_off, Field("note", &Message::NoteOnOff::note, other.note_off.note)),
                  Field("note off", &Message::note_off, Field("velocity", &Message::NoteOnOff::velocity, other.note_off.velocity))),
                  arg, result_listener);
}

MATCHER_P(ControlChangeMatches, other, "")
{
    return ExplainMatchResult(
            AllOf(Field("control change", &Message::control_change, Field("control", &Message::ControlChange::control, other.control_change.control)),
                  Field("control change", &Message::control_change, Field("velocity", &Message::ControlChange::value, other.control_change.value))),
                  arg, result_listener);
}

MATCHER_P(ProgramChangeMatches, other, "")
{
    return ExplainMatchResult(
                  Field("program change", &Message::program_change, Field("number", &Message::ProgramChange::number, other.program_change.number)),
                  arg, result_listener);
}

MATCHER_P(MessageMatches, other, "")
{
    if(arg.type != other.type)
    {
        *result_listener << "Message types don't match, expected " <<
            other.type << " but got " << arg.type;
        return false;
    }

    switch(other.type)
    {
    case NOTE_ON:
        return ExplainMatchResult(
                  NoteOnMatches(other),
                  arg, result_listener);
    case NOTE_OFF:
        return ExplainMatchResult(
                  NoteOffMatches(other),
                  arg, result_listener);
    case CONTROL_CHANGE:
        return ExplainMatchResult(
                  ControlChangeMatches(other),
                  arg, result_listener);
    case PROGRAM_CHANGE:
        return ExplainMatchResult(
                  ProgramChangeMatches(other),
                  arg, result_listener);
    default:
        *result_listener << "Unexpected type " << other.type;
        return false;
        break;
    }

    return false;
}

