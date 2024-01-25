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

#include "audio_param.h"
#include "cmd_handling.h"

namespace {

using testing::_;
using testing::Return;
using testing::StrEq;

using id_t = shrapnel::parameters::id_t;

class MockAudioParameterFloat;

class MockAudioParameters
{
public:
    using MapType = std::map<id_t, std::unique_ptr<MockAudioParameterFloat>>;

    MOCK_METHOD(int, update, (id_t param, float value), ());
    MOCK_METHOD(int,
                create_and_add_parameter,
                (id_t name, float minimum, float maximum, float default_value),
                ());
    MOCK_METHOD(std::atomic<float> *,
                get_raw_parameter,
                (const std::string param),
                ());

    MapType::iterator begin() { return parameters.begin(); };
    MapType::iterator end() { return parameters.end(); };

    MapType parameters;
};

class MockEventSend
{
public:
    MOCK_METHOD(void,
                send,
                (const shrapnel::parameters::ApiMessage &message,
                 std::optional<int> fd));
};

class EventSendAdapter final
{
public:
    explicit EventSendAdapter(MockEventSend &a_event) : event(a_event) {}

    void send(const shrapnel::parameters::ApiMessage &message,
              std::optional<int> fd)
    {
        event.send(message, fd);
    }

private:
    MockEventSend &event;
};

class MockAudioParameterFloat
{
public:
    MockAudioParameterFloat(std::string name, float default_value)
        : value(default_value)
    {
        (void)name;
    }

    MOCK_METHOD(float, get, (), ());

    float *get_raw_parameter() { return &value; }

private:
    float value;
};

class CmdHandling : public ::testing::Test
{
protected:
    CmdHandling()
        : param(std::make_unique<MockAudioParameters>()),
          cmd(param,

              shrapnel::parameters::CommandHandling<MockAudioParameters>::
                  SendMessageCallback::create<EventSendAdapter,
                                              &EventSendAdapter::send>(
                      event_adapter))
    {
    }

    std::shared_ptr<MockAudioParameters> param;
    MockEventSend event;
    EventSendAdapter event_adapter{event};
    shrapnel::parameters::CommandHandling<MockAudioParameters> cmd;

    void dispatch(const shrapnel::parameters::ApiMessage &message, int fd)
    {
        cmd.dispatch(message, fd);
    }
};

TEST_F(CmdHandling, ValidMessage)
{
    EXPECT_CALL(*param, update(id_t("tight"), 1.0f))
        .Times(1)
        .WillRepeatedly(Return(0));

    shrapnel::parameters::Update message{
        .id{"tight"},
        .value{1.f},
    };
    EXPECT_CALL(event, send({message}, testing::Optional(42))).Times(1);

    dispatch(message, 42);
}

TEST_F(CmdHandling, InitialiseParameters)
{
    auto parameter0 = std::make_unique<MockAudioParameterFloat>("test", 0);
    EXPECT_CALL(*parameter0.get(), get()).WillRepeatedly(Return(0));
    auto parameter1 = std::make_unique<MockAudioParameterFloat>("test", 0);
    EXPECT_CALL(*parameter1.get(), get()).WillRepeatedly(Return(1));
    param->parameters["test0"] = std::move(parameter0);
    param->parameters["test1"] = std::move(parameter1);

    shrapnel::parameters::Update expected{
        .id{"test0"},
        .value{0.0f},
    };
    EXPECT_CALL(event, send({expected}, testing::Eq(std::nullopt))).Times(1);

    expected = shrapnel::parameters::Update{
        .id{"test1"},
        .value{1.0f},
    };
    EXPECT_CALL(event, send({expected}, testing::Eq(std::nullopt))).Times(1);

    dispatch(shrapnel::parameters::Initialise{}, 0);
}

} // namespace
