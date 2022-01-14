#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audio_param.h"
#include "cmd_handling.h"

using testing::_;
using testing::Return;

#include "task.h"
#include "queue.h"

template <typename T>
class MockQueue : public shrapnel::QueueBase<T>
{
    public:
    MockQueue(int n) : shrapnel::QueueBase<T>(n) {};
    MOCK_METHOD(BaseType_t, receive, (T *out, TickType_t time_to_wait), (override));
    MOCK_METHOD(BaseType_t, send, (T *out, TickType_t time_to_wait), (override));
};

class MockAudioParameters : public shrapnel::AudioParametersBase
{
    public:
    MOCK_METHOD(esp_err_t, update, (audio_param_t param, float value), (override));
};

class CmdHandling : public ::testing::Test
{
    protected:

    using Message = shrapnel::CommandHandling::Message;

    CmdHandling() : queue(1), cmd(&queue, &param) {}

    MockQueue<Message> queue;
    MockAudioParameters param;

    shrapnel::CommandHandling cmd;
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

TEST_F(CmdHandling, UnregisteredMessage)
{
    Message output = {
        {.json = "{\"id\": \"tight\", \"value\": 1}"},
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
        {.json = "{\"id\": \"tight\", \"value\": 1}"},
    };

    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(
                testing::DoAll(
                    testing::SetArgPointee<0>(output),
                    Return(true)
                ));

    EXPECT_CALL(param, update(PARAM_TIGHT, 1.0f))
        .Times(1)
        .WillRepeatedly(Return(ESP_OK));

    cmd.register_parameter(
            { .name = "tight",
              .id = PARAM_TIGHT,
            });

    cmd.work();
}
