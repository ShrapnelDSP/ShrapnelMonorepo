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

#include "midi_protocol.h"

using testing::InSequence;
using testing::_;

using namespace shrapnel::midi;

class MidiDecoder : public ::testing::Test
{
    protected:

    MidiDecoder() : sut(receiver_fn) {}

    testing::MockFunction<void(shrapnel::midi::Message)> receiver;
    std::function<void(shrapnel::midi::Message)> receiver_fn = receiver.AsStdFunction();
    Decoder sut;
};

TEST_F(MidiDecoder, NoteOn)
{
    Message expected{
        .channel{1},
        .parameters{
            Message::NoteOn{.note{0}, .velocity{1}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0x90,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, NoteOff)
{
    Message expected{
        .channel{1},
        .parameters{
            Message::NoteOff{.note{0}, .velocity{1}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0x80,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, ProgramChange)
{
    Message expected{
        .channel{1},
        .parameters{
            Message::ProgramChange{.number{0}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0xC0,
        0x00,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, NoteOnRunningStatus)
{
    Message expected_first{
        .channel{1},
        .parameters{
            Message::NoteOn{.note{0}, .velocity{1}},
        },
    };

    Message expected_second{
        .channel{1},
        .parameters{
            Message::NoteOn{.note{2}, .velocity{3}},
        },
    };

    InSequence sequence;

    EXPECT_CALL(receiver, Call(expected_first)).Times(1);
    EXPECT_CALL(receiver, Call(expected_second)).Times(1);

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
        .channel{1},
        .parameters{
            Message::ControlChange{.control{0}, .value{1}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0xB0,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, ChannelModeIsNotControlChange)
{
    EXPECT_CALL(receiver, Call(_)).Times(0);

    for (uint8_t second_byte = 0x78; second_byte <= 0x7F; second_byte++)
    {
        std::vector<uint8_t> bytes {
            0xB0,
            second_byte,
            0x01,
        };

        for (auto byte : bytes) {
            sut.decode(byte);
        }
    }
}

TEST_F(MidiDecoder, ChannelNumber)
{
    Message expected{
        .channel{16},
        .parameters{
            Message::NoteOn{.note{0}, .velocity{1}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0x9F,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, IgnoresUnimplementedMessages)
{
    Message expected{
        .channel{1},
        .parameters{
            Message::NoteOn{.note{0}, .velocity{1}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0xE0,
        0x00,
        0x90,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

TEST_F(MidiDecoder, DISABLED_UnimplementedMessageDoesNotUpsetRunningStatus)
{
    /* TODO Make this test pass */
    Message expected{
        .channel{1},
        .parameters{
            Message::NoteOn{.note{0}, .velocity{1}},
        },
    };

    EXPECT_CALL(receiver, Call(expected)).Times(1);

    std::vector<uint8_t> bytes {
        0x90,
        0x00,
        0x01,
        0xE0,
        0x00,
        0x00,
        0x01,
    };

    for (auto byte : bytes) {
        sut.decode(byte);
    }
}

// TODO System exclusive message does not upset decoder
// TODO Bank select:
//      Control Change 0x00 followed by Control change 0x20, finally followed
//      by program change is used to select a program from a 14-bit bank
//      address.
// TODO Real-Time messages may be sent at any time — even between bytes of a
//      message which has a different status.
