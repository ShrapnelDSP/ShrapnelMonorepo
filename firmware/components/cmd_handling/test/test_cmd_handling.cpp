/* cJSON source code is used, it is not mocked
 * esp_log is replaced with a fake that prints to stdout
 */
//#include "cmd_handling.h"

//#include "audio_param.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
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
class MockQueue : public shrapnel::Queue<T>
{
    public:
    MOCK_METHOD(BaseType_t, receive, (T *out, TickType_t time_to_wait), (override));
};


#define MSG_SIZE 128

class CmdHandling : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        cmd_init(&queue);
    }

    MockQueue<char [128]> queue;
};

TEST_F(CmdHandling, QueueFail)
{

#if 0
    mock().expectOneCall("xQueueReceive")
        .ignoreOtherParameters()
        .andReturnValue((int)false);
    mock().expectNoCall("param_update_parameter");
#endif
    EXPECT_CALL(queue, receive(_, portMAX_DELAY))
        .Times(1)
        .WillRepeatedly(Return(false));

    cmd_task_work(NULL);
}

#if 0
TEST(cmd_handling, InvalidMessage)
{
    char output[] = "This is not JSON";

    mock().expectOneCall("xQueueReceive")
        .ignoreOtherParameters()
        .withOutputParameterReturning("pvBuffer", output, MSG_SIZE)
        .andReturnValue((int)true);

    mock().expectNoCall("param_update_parameter");

    cmd_task_work(NULL);
}

TEST(cmd_handling, ValidMessage)
{
    char output[] = "{\"id\": \"tight\", \"value\": 1}";

    mock().expectOneCall("xQueueReceive")
        .ignoreOtherParameters()
        .withOutputParameterReturning("pvBuffer", output, MSG_SIZE)
        .andReturnValue((int)true);

    mock().expectOneCall("param_update_parameter")
        .withParameter("param", PARAM_TIGHT)
        .withParameter("value", 1.0f)
        .andReturnValue(ESP_OK);

    cmd_task_work(NULL);
}
#endif
