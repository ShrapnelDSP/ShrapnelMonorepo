#pragma once

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

namespace shrapnel {

class AudioParameters {
    public:
        virtual esp_err_t update(audio_param_t param, float value) = 0;
};

};
