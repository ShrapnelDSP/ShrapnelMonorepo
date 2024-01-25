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

#include "midi_util.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <utility>

#include "midi_protocol.h"

using testing::Eq;
using testing::Not;

using namespace shrapnel::midi;

class MidiMessage : public ::testing::Test
{
protected:
    MidiMessage() {}
};

TEST_F(MidiMessage, Matcher)
{
    Message message{
        .channel{1},
        .parameters{Message::NoteOff{.note = 2, .velocity = 3}},
    };

    Message copy = message;

    EXPECT_THAT(message, Eq(copy));

    copy.parameters = Message::NoteOn{.note = 2, .velocity = 3};
    EXPECT_THAT(message, Not(Eq(copy)));

    copy = message;
    copy.parameters = Message::NoteOff{.note = 0, .velocity = 3};
    EXPECT_THAT(message, Not(Eq(copy)));

    copy = message;
    copy.parameters = Message::NoteOff{.note = 2, .velocity = 0};
    EXPECT_THAT(message, Not(Eq(copy)));

    copy = message;
    copy.parameters = Message::NoteOn{.note = 2, .velocity = 0};
    EXPECT_THAT(message, Not(Eq(copy)));
}
