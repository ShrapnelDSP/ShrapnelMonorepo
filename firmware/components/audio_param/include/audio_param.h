#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

typedef enum {
    PARAM_TIGHT,
    PARAM_HM2_GAIN,
    PARAM_AMP_GAIN,
    PARAM_BASS,
    PARAM_MIDDLE,
    PARAM_TREBLE,
    PARAM_VOLUME,
    PARAM_GATE_THRESHOLD,
    PARAM_MAX,
} audio_param_t;

esp_err_t param_update_parameter(audio_param_t param, float value);

#ifdef __cplusplus
}
#endif
