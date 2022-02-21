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

/* TODO create a C++ interface for mocking. We can use a shrapnel::QueueBase */
void audio_event_send_callback(const char *message, int fd)
{
    (void)message;
    (void)fd;
};

using testing::_;
using testing::Return;

#include "task.h"
#include "queue.h"

std::atomic<float> *shrapnel::AudioParameterFloat::get_raw_parameter(void)
{
    return nullptr;
}

template <typename T>
class MockQueue : public shrapnel::QueueBase<T>
{
    public:
    MockQueue(int n) : shrapnel::QueueBase<T>(n) {};
    MOCK_METHOD(BaseType_t, receive, (T *out, TickType_t time_to_wait), (override));
    MOCK_METHOD(BaseType_t, send, (T *out, TickType_t time_to_wait), (override));
};

class MockAudioParameters
{
    public:
    using MapType = std::map<std::string, std::unique_ptr<shrapnel::AudioParameterFloat>>;

    MOCK_METHOD(int, update, (std::string param, float value), ());
    MOCK_METHOD(int, create_and_add_parameter, (
        std::string name,
        float minimum,
        float maximum,
        float default_value), ());
    MOCK_METHOD(std::atomic<float> *, get_raw_parameter, (const std::string param), ());
    /* TODO How de we use an actual iterator here?
     *
     *      Can we just not use MOCK_METHOD and implement some code to wrap
     *      begin and end on a member MapType just like the actual
     *      implementation?
     */
    MOCK_METHOD(MapType::iterator, begin, (), ());
    MOCK_METHOD(MapType::iterator, end, (), ());
};

class CmdHandling : public ::testing::Test
{
    protected:

    using Message = shrapnel::CommandHandling<MockAudioParameters>::Message;

    CmdHandling() : queue(1), cmd(&queue, &param) {}

    MockQueue<Message> queue;
    MockAudioParameters param;

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
    Message output = {
        {.json = "This is not JSON"},
        {}
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
    Message output = {
        {.json = "{\"id\": \"tight\", \"value\": 1, \"messageType\": \"parameterUpdate\"}"},
        {}
    };

    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(
                testing::DoAll(
                    testing::SetArgPointee<0>(output),
                    Return(true)
                ));

    EXPECT_CALL(param, update("tight", 1.0f))
        .Times(1)
        .WillRepeatedly(Return(0));

    cmd.work();
}
