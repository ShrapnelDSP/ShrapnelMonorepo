#include "audio_param.h"
#include "CppUTestExt/MockSupport.h"

esp_err_t param_update_parameter(audio_param_t param, float value)
{
    return mock().actualCall("param_update_parameter").
        withParameter("param", param).
        withParameter("value", value).
        returnIntValue();
}
