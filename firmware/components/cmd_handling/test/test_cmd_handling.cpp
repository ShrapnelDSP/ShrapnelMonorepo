/* cJSON source code is used, it is not mocked
 * esp_log is replaced with a fake that prints to stdout
 */
#include "cmd_handling.h"

#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

TEST_GROUP(cmd_handling)
{
    void teardown()
    {
        mock().clear();
    }
};

IMPORT_TEST_GROUP(cmd_handling);

TEST(cmd_handling, QueueFail)
{
    mock().expectOneCall("xQueueReceive").ignoreOtherParameters().andReturnValue((int)false);
    mock().expectNoCall("param_update_parameter");

    cmd_task_work(NULL);

    mock().checkExpectations();
}

/* Queue returns an invalid message, param_update_parameter is not called */
/* Queue returns a valid message, param_update_parameter is called with correct parameters */

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
