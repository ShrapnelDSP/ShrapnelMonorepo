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
#include "midi_util.h"

#include "midi.h"

using testing::InSequence;

using namespace shrapnel::midi;

class MidiDecoder : public ::testing::Test
{
    protected:

    MidiDecoder() : sut(receiver.AsStdFunction()) {}

    testing::MockFunction<void(shrapnel::midi::Message)> receiver;
    Decoder sut;
};

TEST_F(MidiDecoder, NoteOn)
{
    Message expected{
        .type{NOTE_ON},
        .note_on{
            .note{0},
            .velocity{1},
        },
    };

    EXPECT_CALL(receiver, Call(MessageMatches(expected))).Times(1);

    std::vector<uint8_t> bytes {
        0x90,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, NoteOnRunningStatus)
{
    Message expected_first{
        .type{NOTE_ON},
        .note_on{
            .note{0},
            .velocity{1},
        },
    };

    Message expected_second{
        .type{NOTE_ON},
        .note_on{
            .note{2},
            .velocity{3},
        },
    };

    InSequence sequence;

    EXPECT_CALL(receiver, Call(MessageMatches(expected_first))).Times(1);
    EXPECT_CALL(receiver, Call(MessageMatches(expected_second))).Times(1);

    std::vector<uint8_t> bytes {
        0x90,
        0x00,
        0x01,
        0x02,
        0x03,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, ControlChange)
{
    Message expected{
        .type{CONTROL_CHANGE},
        .control_change{
            .control{0},
            .value{1},
        },
    };

    EXPECT_CALL(receiver, Call(MessageMatches(expected))).Times(1);

    std::vector<uint8_t> bytes {
        0xB0,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

// TODO Channel Mode message is not treated as Control Change
// TODO System exclusive message does not upset decoder
// TODO other unknown message does not upset decoder

