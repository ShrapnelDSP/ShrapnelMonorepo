/* cJSON source code is used, it is not mocked
 * esp_log is replaced with a fake that prints to stdout
 */
#include "cJSON.h"
#include <stdio.h>
//#include "CppUTest.h"

/* Queue returns fail, param_update_parameter is not called */
/* Queue returns an invalid message, param_update_parameter is not called */
/* Queue returns a valid message, param_update_parameter is called with correct parameters */
int main(void)
{
    printf("hello world");
    return 0;
}
