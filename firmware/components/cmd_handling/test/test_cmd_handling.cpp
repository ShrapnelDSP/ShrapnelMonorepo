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
    MOCK_METHOD(void, send, (char *json, int fd), (override));
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

    using Message = shrapnel::CommandHandling<MockAudioParameters>::Message;

    CmdHandling() : queue(1), cmd(&queue, &param, event) {}

    MockQueue<Message> queue;
    MockAudioParameters param;
    MockEventSend event;

    shrapnel::CommandHandling<MockAudioParameters> cmd;
};

TEST_F(CmdHandling, QueueFail)
{
    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(Return(false));

    EXPECT_CALL(param, update).Times(0);

    cmd.work();
}

TEST_F(CmdHandling, InvalidMessage)
{
    Message output{
        "This is not JSON",
        0
    };

    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(
                testing::DoAll(
                    testing::SetArgPointee<0>(output),
                    Return(true)
                ));

    EXPECT_CALL(param, update).Times(0);

    cmd.work();
}

TEST_F(CmdHandling, ValidMessage)
{
    Message output{
        "{\"id\": \"tight\", \"value\": 1, \"messageType\": \"parameterUpdate\"}",
        42
    };

    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(
                testing::DoAll(
                    testing::SetArgPointee<0>(output),
                    Return(true)
                ));

    EXPECT_CALL(param, update(id_t("tight"), 1.0f))
        .Times(1)
        .WillRepeatedly(Return(0));

    EXPECT_CALL(event, send(StrEq(output.json), 42)).Times(1);

    cmd.work();
}

TEST_F(CmdHandling, InitialiseParameters)
{
    Message output{
        "{\"messageType\": \"initialiseParameters\"}",
        0
    };

    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(
                testing::DoAll(
                    testing::SetArgPointee<0>(output),
                    Return(true)
                ));

    auto parameter0 = std::make_unique<MockAudioParameterFloat>("test", 0);
    EXPECT_CALL(*parameter0.get(), get()).WillRepeatedly(Return(0));
    auto parameter1 = std::make_unique<MockAudioParameterFloat>("test", 0);
    EXPECT_CALL(*parameter1.get(), get()).WillRepeatedly(Return(1));
    param.parameters["test0"] = std::move(parameter0);
    param.parameters["test1"] = std::move(parameter1);

    const char *message = "{\"id\":\"test0\",\"value\":0}";
    EXPECT_CALL(event, send(StrEq(message), -1)).Times(1);
    message = "{\"id\":\"test1\",\"value\":1}";
    EXPECT_CALL(event, send(StrEq(message), -1)).Times(1);

    cmd.work();
}

}
