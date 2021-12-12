/* cJSON source code is used, it is not mocked
 * esp_log is replaced with a fake that prints to stdout
 */
#include "cmd_handling.h"

#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"

TEST_GROUP(FirstTestGroup)
{
};

IMPORT_TEST_GROUP(FirstTestGroup);

TEST(FirstTestGroup, FirstTest)
{
   FAIL("Fail me!");
}

TEST(FirstTestGroup, SecondTest)
{
    STRCMP_EQUAL("hello", "world");
}

TEST(FirstTestGroup, QueueFail)
{
    /* TODO set up mock queue to fail
     * Expect:
     * - 1 call to queue
     * - 0 calls to output function
     */
    cmd_task_work(NULL);
}

/* Queue returns fail, param_update_parameter is not called */
/* Queue returns an invalid message, param_update_parameter is not called */
/* Queue returns a valid message, param_update_parameter is called with correct parameters */

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
