/* \brief Audio parameter update functions 
 *
 * This file implements a generic audio parameter update function, to abstract
 * the details of individual parameter updates */

#include "audio_param.h"
#include "esp_log.h"
#include "i2s.h"
#include "fmv.h"
#define TAG "audio_param"
#include <assert.h>
#include <math.h>

/* These parameters are all tied to the same processor. All three are required
 * together for an update, so we need to save the state. */
static float _bass;
static float _middle;
static float _treble;

esp_err_t param_update_parameter(audio_param_t param, float value)
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
                //gain range is -40 to 0 dB
                float gain_db = -40 * (1 - value);
                float gain = powf(10.f, gain_db / 20.f);
                ESP_LOGI(TAG, "gain is: %02.1f dB, %01.3f", gain_db, gain);

                i2s_set_gain(gain);
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
        default:
            ESP_LOGE(TAG, "Unhandled parameter %d", param);
            break;
    }

    return ESP_OK;
}
