/* \brief Audio parameter update functions 
 *
 * This file implements a generic audio parameter update function, to abstract
 * the details of individual parameter updates */

#include "audio_param.h"
#include "esp_log.h"
#include "i2s.h"
#include "input_filter.h"
#include "noise_gate.h"
#include "fmv.h"
#define TAG "audio_param"
#include <assert.h>
#include <math.h>

/* These parameters are all tied to the same processor. All three are required
 * together for an update, so we need to save the state. */
static float _bass;
static float _middle;
static float _treble;

static float decibel_to_ratio(float db)
{
    return powf(10.f, db / 20.f);
}

namespace shrapnel {

esp_err_t AudioParameters::update(audio_param_t param, float value)
{
    if(value < 0 || value > 1)
    {
        ESP_LOGE(TAG, "out of range value %f", value);
        return ESP_FAIL;
    }

    if(param < 0 || param > PARAM_MAX)
    {
        ESP_LOGE(TAG, "out of range param %d", param);
        return ESP_FAIL;
    }

    switch(param)
    {
        case PARAM_VOLUME:
            {
                //volume range is -40 to 0 dB
                float volume_db = -40 * (1 - value);
                float volume = decibel_to_ratio(volume_db);
                ESP_LOGI(TAG, "volume is: %02.1f dB, %01.3f", volume_db, volume);

                i2s_set_volume(volume);
                break;
            }
        case PARAM_HM2_GAIN:
            {
                //gain range is 20 to 60 dB
                float gain_db = 20 + (40 * value);
                float gain = decibel_to_ratio(gain_db);
                ESP_LOGI(TAG, "gain is: %02.1f dB, %01.3f", gain_db, gain);

                i2s_set_pedal_gain(gain);
                break;
            }
        case PARAM_AMP_GAIN:
            {
                //gain range is 0 to 30 dB
                float gain_db = 30 * value;
                float gain = decibel_to_ratio(gain_db);
                ESP_LOGI(TAG, "gain is: %02.1f dB, %01.3f", gain_db, gain);

                i2s_set_amp_gain(gain);
                break;
            }
        case PARAM_BASS:
            _bass = value;
            fmv_update_params(_bass, _middle, _treble);
            break;
        case PARAM_MIDDLE:
            _middle = value;
            fmv_update_params(_bass, _middle, _treble);
            break;
        case PARAM_TREBLE:
            _treble = value;
            fmv_update_params(_bass, _middle, _treble);
            break;
        case PARAM_TIGHT:
            filter_set_tight(value > 0.5);
            break;
        case PARAM_GATE_THRESHOLD:
            {
                // -80 dB to 0 dB
                float threshold = -80 + (80 * value);
                gate_set_threshold(threshold, 1);
                break;
            }
        default:
            ESP_LOGE(TAG, "Unhandled parameter %d", param);
            break;
    }

    return ESP_OK;
}

}
