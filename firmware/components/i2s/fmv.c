/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

/*
 *  This file is part of Swedish Chainsaw.
 *  Copyright (C) 2019  Barabas Raffai
 *
 *  Swedish Chainsaw is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  Swedish Chanisaw is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Swedish Chanisaw.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "fmv.h"
#include "iir.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "dsps_mulc.h"

#include "esp_log.h"
#define TAG "fmv"

static float coeffs[8];
static float delay_line[3];
/** This mutex is used to ensure that the coefficients of the filter don't
 * change in the middle of processing */
static SemaphoreHandle_t mutex;
static float sample_rate;

void fmv_update_params(float l, float m, float t)
{
    //temporary buffer used to minimise the time spent with the mutex
    float tmp_coeffs[8];

    //JCM800 values
    float R1 = 220e3;
    float R2 = 1e6;
    float R3 = 22e3;
    float R4 = 33e3;

    float C1 = 470e-12;
    float C2 = 22e-9;
    float C3 = 22e-9;

    float Fs = sample_rate;

    float b1 = t * C1 * R1 + m * C3 * R3 + l * (C1 * R2 + C2 * R2) + 
        (C1 * R3 + C2 * R3);

    float b2 = t * (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 ) 
        - m * m * (C1 * C3 * R3 * R3  + C2 * C3 * R3 * R3 )
        + m * (C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3)
        + l * (C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4)
        + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3)
        + (C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4);

    float b3 = l*m*(C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4 )
        - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4 )
        + m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4 )
        + t * C1 * C2 * C3 * R1 * R3 * R4 - t * m * C1 * C2 * C3 * R1 * R3 * R4
        + t * l * C1 * C2 * C3 * R1 * R2 * R4;

    float a0 = 1;

    float a1 = (C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4 )
        + m * C3 * R3 + l * (C1 * R2 + C2 * R2);

    float a2 = m * (C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3
            + C2 * C3 * R3 * R3 ) + l * m * (C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3 )
        - m*m * (C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3 ) + l*(C1 * C2 * R2 * R4
                + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4 )
        + (C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4
                + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4 );

    float a3 = l*m*(C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4 )
        - m * m * (C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4)
        + m * (C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 
                - C1 * C2 * C3 * R1 * R3 * R4 ) 
        + l * C1 * C2 * C3 * R1 * R2 * R4 + C1 * C2 * C3 * R1 * R3 * R4;

    //bilinear transform
    float c = 2*Fs;
    float c2 = c*c;
    float c3 = c*c*c;

    float A0 = -a0 - a1 * c- a2 * c2 - a3 * c3;

    tmp_coeffs[0] = (-b1 * c- b2 * c2 - b3 * c3) / A0;
    tmp_coeffs[1] = (-b1 * c+ b2 * c2 + 3 * b3 * c3) / A0;
    tmp_coeffs[2] = (b1 * c+ b2 * c2 - 3 * b3 * c3) / A0;
    tmp_coeffs[3] = (b1 * c- b2 * c2 + b3 * c3) / A0;

    tmp_coeffs[4] = 1;
    tmp_coeffs[5] = (-3 * a0 - a1 * c+ a2 * c2 + 3 * a3 * c3) / A0;
    tmp_coeffs[6] = (-3 * a0 + a1 * c+ a2 * c2 - 3 * a3 * c3) / A0;
    tmp_coeffs[7] = (-a0 + a1 * c- a2 * c2 + a3 * c3) / A0;

    if(xSemaphoreTake(mutex, 100 / portTICK_PERIOD_MS) == pdPASS)
    {
        for(int i = 0; i < 8; i++)
        {
            coeffs[i] = tmp_coeffs[i];
        }
        xSemaphoreGive(mutex);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to take semaphore, parameter update skipped");
    }

    for(int i = 0; i < 8; i++)
    {
        ESP_LOGD(TAG, "coeffs[%d] = %f", i, (double)coeffs[i]);
    }
}

void fmv_process(float *buf, size_t buf_len)
{
    if(xSemaphoreTake(mutex, 10 / portTICK_PERIOD_MS) == pdPASS)
    {
        iir_process(buf, buf, buf_len, coeffs, delay_line,
                sizeof(coeffs)/sizeof(coeffs[0]));
        xSemaphoreGive(mutex);
    }
    else
    {
        // If we can't perform normal processing, output silence. This makes
        // the error more noticable. 
        dsps_mulc_f32_ae32(buf, buf, buf_len, 0.f, 1, 1);
    }
}

esp_err_t fmv_init(float a_sample_rate)
{
    for(int i = 0; i < sizeof(delay_line)/sizeof(delay_line[0]); i++)
    {
        delay_line[i] = 0.f;
    }

    mutex = xSemaphoreCreateMutex();

    if(mutex == NULL)
    {
        return ESP_FAIL;
    }

    xSemaphoreGive(mutex);

    sample_rate = a_sample_rate;
    fmv_update_params(0.5, 0.5, 0.5);

    return ESP_OK;
}
