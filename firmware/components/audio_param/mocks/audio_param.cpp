#include "audio_param.h"

esp_err_t param_update_parameter(audio_param_t param, float value)
{
#if 0
    return mock().actualCall("param_update_parameter").
        withParameter("param", param).
        withParameter("value", value).
        returnIntValue();
#endif
    return ESP_OK;
}
