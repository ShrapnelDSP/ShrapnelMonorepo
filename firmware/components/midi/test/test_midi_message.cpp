/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "midi.h"

using testing::_;
using testing::AllOf;
using testing::Field;
using testing::ExplainMatchResult;
using testing::Not;

using namespace shrapnel::midi;

MATCHER_P(MessageMatches, other, "")
{
    return ExplainMatchResult(
            AllOf(Field("type", &Message::type, other.type),
                  Field("note on", &Message::note_on, Field("note", &NoteOnOff::note, other.note_on.note)),
                  Field("note on", &Message::note_on, Field("velocity", &NoteOnOff::velocity, other.note_on.velocity))),
                  arg, result_listener);

}


class MidiMessage : public ::testing::Test
{
    protected:

    MidiMessage() {}
};

TEST_F(MidiMessage, Equality)
{
    Message message{
        .type{NOTE_OFF},
        .note_on{
            .note{0},
            .velocity{1}
        },
    };

    Message copy = message;

    EXPECT_THAT(message, MessageMatches(copy));
    EXPECT_THAT(message, MessageMatches(copy));

    copy.type = NOTE_ON;
    EXPECT_THAT(message, Not(MessageMatches(copy)));

    copy = message;
    copy.note_on.note = 1;
    EXPECT_THAT(message, Not(MessageMatches(copy)));

    copy = message;
    copy.note_on.velocity = 0;
    EXPECT_THAT(message, Not(MessageMatches(copy)));
}
