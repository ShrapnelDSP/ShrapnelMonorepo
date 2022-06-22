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

using namespace shrapnel::midi;

class MidiDecoder : public ::testing::Test
{
    protected:

    MidiDecoder() : sut(receiver.AsStdFunction()) {}

    testing::MockFunction<void(shrapnel::midi::Message)> receiver;
    Decoder sut;
};

#if 0
TEST_F(MidiDecoder, NoteOn)
{
    Message expected{
        .type{CHANNEL_VOICE},
        .u{
            .voice{
                .type = NOTE_ON,
                .u{
                    .note_on{
                        .note{0},
                        .velocity{0}
                    }
                }
            }
        }
    };

    EXPECT_CALL(receiver, Call(expected))
        .Times(1);

    std::vector<uint8_t> bytes {
        0x90,
        0x00,
        0x01,
    };

    for (auto byte : bytes){
        sut.decode(byte);
    }
}
#endif
