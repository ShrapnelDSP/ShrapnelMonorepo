/* cJSON source code is used, it is not mocked
 * esp_log is replaced with a fake that prints to stdout
 */
#include "cmd_handling.h"

#include "audio_param.h"

#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

#define MSG_SIZE 128

TEST_GROUP(cmd_handling)
{
    void setup()
    {
        cmd_init(0, MSG_SIZE);
    }

    void teardown()
    {
        mock().clear();
    }
};

IMPORT_TEST_GROUP(cmd_handling);

TEST(cmd_handling, QueueFail)
{
    mock().expectOneCall("xQueueReceive")
        .ignoreOtherParameters()
        .andReturnValue((int)false);
    mock().expectNoCall("param_update_parameter");

    cmd_task_work(NULL);

    mock().checkExpectations();
}

TEST(cmd_handling, InvalidMessage)
{
    char output[] = "This is not JSON";

    mock().expectOneCall("xQueueReceive")
        .ignoreOtherParameters()
        .withOutputParameterReturning("pvBuffer", output, MSG_SIZE)
        .andReturnValue((int)true);

    mock().expectNoCall("param_update_parameter");

    cmd_task_work(NULL);

    mock().checkExpectations();
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

    mock().checkExpectations();
}

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
