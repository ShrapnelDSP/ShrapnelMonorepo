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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <memory>

#include "audio_param.h"
#include "midi_handling.h"

namespace {

using namespace shrapnel;
using shrapnel::midi::Mapping;
using shrapnel::midi::Message;

using id_t = shrapnel::parameters::id_t;
using ::testing::FloatEq;
using ::testing::Not;
using ::testing::Return;
using ::testing::StrictMock;

class MockAudioParameters
{
public:
    MOCK_METHOD(int, update, (id_t param, float value), ());
    MOCK_METHOD(float, get, (const id_t &param), ());
};

class MockPresetLoader
{
public:
    MOCK_METHOD(int, load_preset, (presets::id_t id), ());
};

TEST(MidiHandling, ProcessParameter)
{
    auto mappings =
        std::make_shared<std::map<midi::Mapping::id_t, midi::Mapping>>();
    auto parameters_mock = std::make_shared<MockAudioParameters>();
    auto preset_loader = std::make_shared<StrictMock<MockPresetLoader>>();

    auto sut = MidiMessageHandler(parameters_mock, mappings, preset_loader);

    (*mappings)[0] = {
        .midi_channel{1},
        .cc_number{2},
        .mode{Mapping::Mode::PARAMETER},
        .parameter_name{"gain"},
    };

    EXPECT_CALL(*parameters_mock, update(id_t("gain"), 0.f));
    sut.process_message({
        .channel{1},
        .parameters{Message::ControlChange{.control = 2, .value = 0}},
    });

    EXPECT_CALL(*parameters_mock, update(id_t("gain"), 1.f));
    sut.process_message({
        .channel{1},
        .parameters{Message::ControlChange{
            .control = 2, .value = shrapnel::midi::CC_VALUE_MAX}},
    });

    EXPECT_CALL(*parameters_mock, update).Times(0);
    sut.process_message({
        .channel{99},
        .parameters{Message::ControlChange{.control = 2, .value = 0}},
    });

    sut.process_message({
        .channel{1},
        .parameters{Message::ControlChange{.control = 99, .value = 0}},
    });
}

TEST(MidiHandling, ProcessToggle)
{
    auto mappings =
        std::make_shared<std::map<midi::Mapping::id_t, midi::Mapping>>();
    auto parameters_mock = std::make_shared<MockAudioParameters>();
    auto preset_loader = std::make_shared<StrictMock<MockPresetLoader>>();

    auto sut = MidiMessageHandler(parameters_mock, mappings, preset_loader);

    auto process_message_with_value = [&](uint8_t value)
    {
        sut.process_message({
            .channel{1},
            .parameters{Message::ControlChange{.control = 2, .value = value}},
        });
    };

    (*mappings)[1] = {
        .midi_channel{1},
        .cc_number{2},
        .mode{Mapping::Mode::TOGGLE},
        .parameter_name{"bypass"},
    };

    EXPECT_CALL(*parameters_mock, get(id_t{"bypass"}))
        .WillRepeatedly(Return(0.f));
    {
        ::testing::InSequence seq;

        // update is called for every event with non-zero value
        // mock always returns 0, so call should be with 1 always
        EXPECT_CALL(*parameters_mock, update(id_t("bypass"), FloatEq(1.f)))
            .Times(2);
        EXPECT_CALL(*parameters_mock, update).Times(0);
    }

    process_message_with_value(0);
    process_message_with_value(1);
    process_message_with_value(2);

    sut.process_message({
        .channel{99},
        .parameters{Message::ControlChange{.control = 2, .value = 1}},
    });

    sut.process_message({
        .channel{1},
        .parameters{Message::ControlChange{.control = 99, .value = 1}},
    });
}

TEST(MidiHandling, ProcessPresetButton)
{
    auto mappings =
        std::make_shared<std::map<midi::Mapping::id_t, midi::Mapping>>();
    auto parameters_mock = std::make_shared<MockAudioParameters>();
    auto preset_loader = std::make_shared<StrictMock<MockPresetLoader>>();

    auto sut = MidiMessageHandler(parameters_mock, mappings, preset_loader);

    (*mappings)[0] = {
        .midi_channel{5},
        .cc_number{42},
        .mode{Mapping::Mode::BUTTON},
        .preset_id{3},
    };

    // these messages are ignored
    sut.process_message({
        .channel{5},
        .parameters{Message::ControlChange{.control = 42,
                                           .value = midi::CC_VALUE_MAX - 1}},
    });

    sut.process_message({
        .channel{5},
        .parameters{
            Message::ControlChange{.control = 41, .value = midi::CC_VALUE_MAX}},
    });

    sut.process_message({
        .channel{6},
        .parameters{
            Message::ControlChange{.control = 41, .value = midi::CC_VALUE_MAX}},
    });

    EXPECT_CALL(*preset_loader, load_preset(3)).Times(1);
    sut.process_message({
        .channel{5},
        .parameters{
            Message::ControlChange{.control = 42, .value = midi::CC_VALUE_MAX}},
    });
}

} // namespace