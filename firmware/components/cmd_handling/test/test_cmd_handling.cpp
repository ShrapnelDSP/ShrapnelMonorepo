#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audio_param.h"
#include "cmd_handling.h"

using testing::_;
using testing::Return;

#include "task.h"
#include "queue.h"

#if 0
template<shrapnel::task_func_t init_function, shrapnel::task_func_t work_function>
class MockTask : public shrapnel::Task<init_function, work_function> {
};
#endif

template <typename T>
class MockQueue : public shrapnel::QueueBase<T>
{
    public:
    MockQueue(int n) : shrapnel::QueueBase<T>(n) {};
    MOCK_METHOD(BaseType_t, receive, (T *out, TickType_t time_to_wait), (override));
};

class MockAudioParameters : public shrapnel::AudioParameters
{
    public:
    MOCK_METHOD(esp_err_t, update, (audio_param_t param, float value), (override));
};

class CmdHandling : public ::testing::Test
{
    protected:
    CmdHandling() : queue(1) {}

    void SetUp() override
    {
        cmd_init(&queue, &param);
    }

    MockQueue<cmd_message_t> queue;
    MockAudioParameters param;
};

TEST_F(CmdHandling, QueueFail)
{

    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(Return(false));

    EXPECT_CALL(param, update).Times(0);

    cmd_task_work(NULL);
}

TEST_F(CmdHandling, InvalidMessage)
{
    cmd_message_t output = {
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

    cmd_task_work(NULL);
}

TEST_F(CmdHandling, ValidMessage)
{
    cmd_message_t output = {
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

    cmd_task_work(NULL);
}
