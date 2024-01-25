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

#include "driver/i2s.h"
#include "audio_processor.h"
#include "float_convert.h"
#include "hardware.h"
#include "i2s.h"

#include "esp_log.h"

#include <math.h> //for roundf
#include <stdint.h>
#include <stdio.h>

#include "audio_events.h"
#include "freertos/task.h"
#include "profiling.h"

#define DMA_BUF_COUNT (3)

#define TASK_STACK (35000)

/* Guarantee no preemption by esp-idf tasks.
 *
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html#choosing-application-task-priorities
 */
#define TASK_PRIO (19)

#define I2S_QUEUE_SIZE (4 * DMA_BUF_COUNT)

#define TAG "codec_i2s"

namespace shrapnel::audio {

QueueHandle_t i2s_queue;
gpio_num_t g_profiling_gpio = static_cast<gpio_num_t>(-1);

namespace {

int32_t rx_buf[2 * DMA_BUF_SIZE];

AudioProcessorParameters
parameter_init(parameters::AudioParametersBase *audio_params);
void process_samples(std::span<int32_t, 2 * DMA_BUF_SIZE> buf,
                     AudioProcessor &processor);

} // namespace

void log_i2s_event(const i2s_event_t &e)
{
    switch(e.type)
    {
    case I2S_EVENT_DMA_ERROR:
        ESP_LOGE(TAG, "Something went wrong with DMA");
        break;
    case I2S_EVENT_TX_DONE:
    case I2S_EVENT_RX_DONE:
        //do nothing
        break;
    case I2S_EVENT_TX_Q_OVF:
        ESP_LOGE(TAG, "TX queue overflowed");
        break;
    case I2S_EVENT_RX_Q_OVF:
        ESP_LOGE(TAG, "RX queue overflowed");
        break;
    default:
        ESP_LOGE(TAG, "Unhandled type: %d", e.type);
        break;
    }
}

static void i2s_processing_task(void *param)
{
    (void)param;
    auto audio_param =
        reinterpret_cast<shrapnel::parameters::AudioParametersBase *>(param);
    size_t tx_rx_size;

    i2s_config_t i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX |
                                        I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_SIZE,
        .use_apll = true,
        .tx_desc_auto_clear =
            true,        //this will clear the tx buffer when we overrun
        .fixed_mclk = 0, // Use mclk_multiple
        .mclk_multiple = I2S_MCLK_MULTIPLE_384,
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT,
    };

    i2s_pin_config_t pin_config = {.mck_io_num = I2S_MCK_IO,
                                   .bck_io_num = I2S_BCK_IO,
                                   .ws_io_num = I2S_WS_IO,
                                   .data_out_num = I2S_DO_IO,
                                   .data_in_num = I2S_DI_IO};

    esp_err_t err = i2s_driver_install(static_cast<i2s_port_t>(I2S_NUM),
                                       &i2s_config,
                                       I2S_QUEUE_SIZE,
                                       &i2s_queue);
    if(err != ESP_OK)
    {
        ESP_LOGE(
            TAG, "i2s_driver_install failed %d, %s", err, esp_err_to_name(err));
        return;
    }
    ESP_LOGD(TAG, "queue address: %p", (void *)i2s_queue);

    err = i2s_set_pin(static_cast<i2s_port_t>(I2S_NUM), &pin_config);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_set_pin failed %d, %s", err, esp_err_to_name(err));
        return;
    }

    auto processor_parameters = parameter_init(audio_param);
    auto processor = shrapnel::AudioProcessor(processor_parameters);
    processor.prepare(SAMPLE_RATE, DMA_BUF_SIZE);

    /* Get the DMA buffers into their initial states
     *
     * The RX buffers should be all empty, and the TX buffers should all be
     * full.
     */
    err = i2s_stop(static_cast<i2s_port_t>(I2S_NUM));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_stop failed %d, %s", err, esp_err_to_name(err));
        return;
    }

    size_t read_count;
    do
    {
        err = i2s_read(static_cast<i2s_port_t>(I2S_NUM),
                       rx_buf,
                       DMA_BUF_SIZE,
                       &read_count,
                       100);
        if(err != ESP_OK)
        {
            ESP_LOGE(TAG, "i2s_read failed %d, %s", err, esp_err_to_name(err));
            return;
        }
        ESP_LOGI(TAG, "discarded %zu samples from RX buffer", read_count);
    } while(read_count != 0);

    err = i2s_zero_dma_buffer(static_cast<i2s_port_t>(I2S_NUM));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,
                 "i2s_zero_dma_buffer failed %d, %s",
                 err,
                 esp_err_to_name(err));
        return;
    }

    err = i2s_start(static_cast<i2s_port_t>(I2S_NUM));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_start failed %d, %s", err, esp_err_to_name(err));
        return;
    }

    while(true)
    {
#if !defined(GENERATE_SINE_WAVE) && !defined(GENERATE_RAMP)
        i2s_read(static_cast<i2s_port_t>(I2S_NUM),
                 rx_buf,
                 sizeof(rx_buf),
                 &tx_rx_size,
                 100 / portTICK_PERIOD_MS);
        if(tx_rx_size != sizeof(rx_buf))
        {
            ESP_LOGE(TAG,
                     "Got the wrong number of bytes %zu/%zu",
                     tx_rx_size,
                     sizeof(rx_buf));
        }
#elif defined(GENERATE_SINE_WAVE)
        int len = sizeof(rx_buf) / sizeof(rx_buf[0]);
        for(int i = 0; i < len; i++)
        {
            rx_buf[i] =
                float_to_int32(sinf(2 * ((float)M_PI) * i / (float)len));
        }
#elif defined(GENERATE_RAMP)
        int len = sizeof(rx_buf) / sizeof(rx_buf[0]);
        for(int i = 0; i < len; i++)
        {
            rx_buf[i] = (i - len / 2) * (INT32_MAX / (len / 2));
        }
#endif

        process_samples(rx_buf, processor);

        i2s_write(static_cast<i2s_port_t>(I2S_NUM),
                  rx_buf,
                  sizeof(rx_buf),
                  &tx_rx_size,
                  100 / portTICK_PERIOD_MS);
        if(tx_rx_size != sizeof(rx_buf))
        {
            ESP_LOGE(TAG,
                     "Sent the wrong number of bytes %zu/%zu",
                     tx_rx_size,
                     sizeof(rx_buf));
        }
    }
}

esp_err_t i2s_setup(gpio_num_t profiling_gpio,
                    shrapnel::parameters::AudioParametersBase *audio_param)
{
    esp_err_t err;

    g_profiling_gpio = profiling_gpio;
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << profiling_gpio,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    err = gpio_config(&io_conf);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_config failed %d, %s", err, esp_err_to_name(err));
        return err;
    }
    err = gpio_set_level(profiling_gpio, 0);
    if(err != ESP_OK)
    {
        ESP_LOGE(
            TAG, "gpio_set_level failed %d, %s", err, esp_err_to_name(err));
        return err;
    }

    // This task must be pinned as CPU cycle count is used for profiling
    int ret = xTaskCreatePinnedToCore(i2s_processing_task,
                                      "i2s proc",
                                      TASK_STACK,
                                      audio_param,
                                      TASK_PRIO,
                                      NULL,
                                      1);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Processing task create failed %d", ret);
    }

    return ESP_OK;
}

namespace {
void process_samples(std::span<int32_t, 2 * DMA_BUF_SIZE> buf,
                     AudioProcessor &processor)
{
    assert(g_profiling_gpio != -1 && "I2S task has not been initialised");
    static float fbuf[DMA_BUF_SIZE];

    gpio_set_level(g_profiling_gpio, 1);
    profiling_start();
    /* Copy samples, skipping every second one. These correspond to the left
     * channel of the ADC and DAC, which are not used here
     * XXX not sure why the right channel is not at index 0 */
    for(size_t i = 1; i < buf.size(); i += 2)
    {
        if(buf[i] == INT32_MIN || buf[i] == INT32_MAX)
        {
            shrapnel::events::input_clipped.clear();
            shrapnel::events::input_clipped.notify_all();
        }
        fbuf[i / 2] = buf[i] / (float)INT32_MAX;
    }

    // call process on processor
    processor.process(fbuf);

    for(size_t i = 1; i < buf.size(); i += 2)
    {
        if(fbuf[i / 2] > 1.f || fbuf[i / 2] < -1.f)
        {
            shrapnel::events::output_clipped.clear();
            shrapnel::events::output_clipped.notify_all();
        }

        if(fbuf[i / 2] != fbuf[i / 2])
        {
            ESP_LOGE(TAG, "Not a number at index %d", i);
            assert(0);
        }

        buf[i] = float_to_int32(fbuf[i / 2]);
    }
    profiling_mark_stage(20);

    profiling_stop();
    gpio_set_level(g_profiling_gpio, 0);

    ESP_LOGD(TAG, "stack: %d", uxTaskGetStackHighWaterMark(nullptr));
}

AudioProcessorParameters
parameter_init(shrapnel::parameters::AudioParametersBase *audio_params)
{
    return {
        .amplifier{
            .gain{audio_params->get_raw_parameter("ampGain")},
            .channel{audio_params->get_raw_parameter("ampChannel")},
            .bass{audio_params->get_raw_parameter("bass")},
            .middle{audio_params->get_raw_parameter("middle")},
            .treble{audio_params->get_raw_parameter("treble")},
            .contour{audio_params->get_raw_parameter("contour")},
            .volume{audio_params->get_raw_parameter("volume")},
        },
        .gate{
            .threshold{audio_params->get_raw_parameter("noiseGateThreshold")},
            .hysteresis{audio_params->get_raw_parameter("noiseGateHysteresis")},
            .attack{audio_params->get_raw_parameter("noiseGateAttack")},
            .hold{audio_params->get_raw_parameter("noiseGateHold")},
            .release{audio_params->get_raw_parameter("noiseGateRelease")},
            .bypass{audio_params->get_raw_parameter("noiseGateBypass")},
        },
        .chorus{
            .rate{audio_params->get_raw_parameter("chorusRate")},
            .depth{audio_params->get_raw_parameter("chorusDepth")},
            .mix{audio_params->get_raw_parameter("chorusMix")},
            .bypass{audio_params->get_raw_parameter("chorusBypass")},
        },
        .wah{
            .position{audio_params->get_raw_parameter("wahPosition")},
            .vocal{audio_params->get_raw_parameter("wahVocal")},
            .bypass{audio_params->get_raw_parameter("wahBypass")},
        },
    };
}
} // namespace

} // namespace shrapnel::audio
