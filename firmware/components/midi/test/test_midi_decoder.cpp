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

testing::MockFunction<void(shrapnel::midi::Message)> receiver;

class MidiDecoder : public ::testing::Test
{
    protected:

    using Decoder = shrapnel::midi::Decoder;

    MidiDecoder() : sut(receiver.AsStdFunction()) {}

    Decoder sut;
};

TEST_F(MidiDecoder, NoteOn)
{
    EXPECT_CALL(receiver, Call(_)).Times(1);

    std::vector<uint8_t> bytes {
        0x90,
        0x00,
        0x01,
    };

    for (auto byte : bytes){
        sut.decode(byte);
    }
}
