#include "input_filter.h"
#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "dsps_mulc.h"
#include "dsps_biquad.h"
#include "dsps_biquad_gen.h"
#include <math.h>

#include "esp_log.h"
#define TAG "fmv"

static float pedal_coeffs_normal[5];
static float pedal_coeffs_tight[5];
static float pedal_delay[3];
static bool pedal_tight;

static float amp_coeffs[5];
static float amp_delay[3];

static float final_coeffs[5];
static float final_delay[3];

/** Switch the pedal input to tight setting.
 *
 * \todo Check for glitches while switching the tight option. The delay line is
 * shared, which might be an issue.
 */
void filter_set_tight(bool t)
{
    pedal_tight = t;
}

/** Process the pedal input filter.
 *
 * Two options are provided:
 * - HM2 based filter, 60Hz high pass, 1st order
 * - Tighter filter, 700Hz high pass, 1st order
 */
void filter_pedal_input_process(float *buf, size_t buf_len)
{
    if(pedal_tight)
    {
        dsps_biquad_f32_ae32(buf, buf, buf_len, pedal_coeffs_tight,
                pedal_delay);
    }
    else
    {
        dsps_biquad_f32_ae32(buf, buf, buf_len, pedal_coeffs_normal,
                pedal_delay);
    }
}

/** Process the amplifier input filter.
 *
 * 600 Hz high pass
 */
void filter_amp_input_process(float *buf, size_t buf_len)
{
    dsps_biquad_f32_ae32(buf, buf, buf_len, amp_coeffs, amp_delay);
}

/** Process the final filter.
 *
 * 7 kHz low pass to make the sound less harsh.
 */
void filter_final_process(float *buf, size_t buf_len)
{
    dsps_biquad_f32_ae32(buf, buf, buf_len, final_coeffs, final_delay);
}

/** Initialise the filer based effects.
 *
 * \todo pedal and amp filter should be 1st order, but only 2nd order design
 * functions are provided */
esp_err_t filter_init(void)
{
    esp_err_t err = ESP_OK;

    err = dsps_biquad_gen_hpf_f32(pedal_coeffs_normal, 60.f/SAMPLE_RATE, 1.f/sqrtf(2.f));

    if(err != ESP_FAIL)
    {
        err = dsps_biquad_gen_hpf_f32(pedal_coeffs_tight, 700.f/SAMPLE_RATE, 1.f/sqrtf(2.f));
    }

    if(err != ESP_FAIL)
    {
        err = dsps_biquad_gen_hpf_f32(amp_coeffs, 600.f/SAMPLE_RATE, 1.f/sqrtf(2.f));
    }

    if(err != ESP_FAIL)
    {
        err = dsps_biquad_gen_lpf_f32(final_coeffs, 7000.f/SAMPLE_RATE, 1.f/sqrtf(2.f));
    }

    for(int i = 0; i < 5; i++)
    {
        ESP_LOGI(TAG, "pedal coeffs normal[%d] = %f", i, (double)pedal_coeffs_normal[i]);
    }

    for(int i = 0; i < 5; i++)
    {
        ESP_LOGI(TAG, "pedal coeffs tight[%d] = %f", i, (double)pedal_coeffs_tight[i]);
    }

    for(int i = 0; i < 5; i++)
    {
        ESP_LOGI(TAG, "amp coeffs[%d] = %f", i, (double)amp_coeffs[i]);
    }

    for(int i = 0; i < 5; i++)
    {
        ESP_LOGI(TAG, "final coeffs[%d] = %f", i, (double)final_coeffs[i]);
    }

    return err;
}
