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

#include "audio_param.h"
#include "cmd_handling.h"

#include "audio_events.h"

#include "task.h"
#include "queue.h"

namespace {

using testing::_;
using testing::Return;
using testing::StrEq;

using id_t = shrapnel::parameters::id_t;

template <typename T>
class MockQueue : public shrapnel::QueueBase<T>
{
    public:
    MockQueue(int n) : shrapnel::QueueBase<T>(n) {};
    MOCK_METHOD(BaseType_t, receive, (T *out, TickType_t time_to_wait), (override));
    MOCK_METHOD(BaseType_t, send, (T *out, TickType_t time_to_wait), (override));
};

class MockAudioParameterFloat;

class MockAudioParameters
{
    public:
    using MapType = std::map<id_t, std::unique_ptr<MockAudioParameterFloat>>;

    MOCK_METHOD(int, update, (id_t param, float value), ());
    MOCK_METHOD(int, create_and_add_parameter, (
        id_t name,
        float minimum,
        float maximum,
        float default_value), ());
    MOCK_METHOD(std::atomic<float> *, get_raw_parameter, (const std::string param), ());

    MapType::iterator begin() { return parameters.begin(); };
    MapType::iterator end() { return parameters.end(); };

    MapType parameters;
};

class MockEventSend : public shrapnel::EventSendBase
{
    public:
    MOCK_METHOD(void, send, (const char *json, int fd), (override));
    MOCK_METHOD(void, send, (const char *json), (override));
};

class MockAudioParameterFloat
{
    public:
    MockAudioParameterFloat(std::string name, float default_value) :
        value(default_value)
    {
        (void)name;
    }

    MOCK_METHOD(float, get, (), ());

    float *get_raw_parameter(void)
    {
        return &value;
    }

    private:
    float value;
};

class CmdHandling : public ::testing::Test
{
    protected:

    using Message = shrapnel::parameters::CommandHandling<MockAudioParameters>::Message;

    CmdHandling() : cmd(&param, event) {}

    MockAudioParameters param;
    MockEventSend event;

    shrapnel::parameters::CommandHandling<MockAudioParameters> cmd;
};

TEST_F(CmdHandling, InvalidMessage)
{
    Message message{
        "This is not JSON",
        0
    };

    EXPECT_CALL(param, update).Times(0);
    cmd.dispatch(message);
}

TEST_F(CmdHandling, ValidMessage)
{
    Message message{
        R"({"id":"tight","value":1.0,"messageType":"parameterUpdate"})",
        42
    };

    EXPECT_CALL(param, update(id_t("tight"), 1.0f))
        .Times(1)
        .WillRepeatedly(Return(0));

    EXPECT_CALL(event, send(StrEq(message.json), 42)).Times(1);

    cmd.dispatch(message);
}

TEST_F(CmdHandling, InitialiseParameters)
{
    Message message{
        R"({"messageType": "initialiseParameters"})",
        0
    };

    auto parameter0 = std::make_unique<MockAudioParameterFloat>("test", 0);
    EXPECT_CALL(*parameter0.get(), get()).WillRepeatedly(Return(0));
    auto parameter1 = std::make_unique<MockAudioParameterFloat>("test", 0);
    EXPECT_CALL(*parameter1.get(), get()).WillRepeatedly(Return(1));
    param.parameters["test0"] = std::move(parameter0);
    param.parameters["test1"] = std::move(parameter1);

    const char *expected = R"({"id":"test0","value":0.0,"messageType":"parameterUpdate"})";
    EXPECT_CALL(event, send(StrEq(expected), -1)).Times(1);
    expected = R"({"id":"test1","value":1.0,"messageType":"parameterUpdate"})";
    EXPECT_CALL(event, send(StrEq(expected), -1)).Times(1);

    cmd.dispatch(message);
}

}
