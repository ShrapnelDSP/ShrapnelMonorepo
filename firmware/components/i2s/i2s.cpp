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
#include "i2s.h"
#include "process.h"
#include "float_convert.h"
#include "hardware.h"

#include "esp_log.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>//for roundf

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define BITS            (I2S_BITS_PER_SAMPLE_24BIT)
#define MCK             (384*SAMPLE_RATE)

#define DMA_BUF_COUNT   (3)

#define TASK_STACK      (3000)
#define TASK_PRIO       (5)

#define I2S_QUEUE_SIZE (4*DMA_BUF_COUNT)
static QueueHandle_t i2s_queue;

#define TAG "codec_i2s"

#if BITS == I2S_BITS_PER_SAMPLE_24BIT
static int32_t rx_buf[2*DMA_BUF_SIZE];
#define SAMPLE_MAX INT32_MAX
#elif BITS == I2S_BITS_PER_SAMPLE_16BIT
static int16_t rx_buf[2*DMA_BUF_SIZE];
#define SAMPLE_MAX INT16_MAX
#else
#error "Unsupported I2S bit width"
#endif

gpio_num_t g_profiling_gpio = static_cast<gpio_num_t>(-1);

static inline void log_event(i2s_event_t e)
{
    ESP_LOGI(TAG, "type: %d, size: %d", e.type, e.size);
}

static void event_task(void *param)
{
    i2s_event_t e;
    while(1)
    {
        xQueueReceive(i2s_queue, &e, portMAX_DELAY);
        switch(e.type)
        {
            case I2S_EVENT_DMA_ERROR:
                ESP_LOGE(TAG, "Something went wrong with DMA");
                break;
            case I2S_EVENT_TX_DONE:
            case I2S_EVENT_RX_DONE:
                //do nothing
                break;
            default:
                ESP_LOGE(TAG, "Unhandled type: %d", e.type);
                break;
        }
    }
}

static void i2s_processing_task(void *param)
{
    size_t tx_rx_size;

    while(1)
    {
#if !defined(GENERATE_SINE_WAVE) && !defined(GENERATE_RAMP)
        i2s_read(static_cast<i2s_port_t>(I2S_NUM), rx_buf, sizeof(rx_buf), &tx_rx_size, 100/portTICK_PERIOD_MS);
        if(tx_rx_size != sizeof(rx_buf))
        {
            ESP_LOGE(TAG, "Got the wrong number of bytes %d/%d", tx_rx_size, sizeof(rx_buf));
        }
#elif defined(GENERATE_SINE_WAVE)
        int len = sizeof(rx_buf) / sizeof(rx_buf[0]);
        for(int i = 0; i < len; i++)
        {
            rx_buf[i] = float_to_int32(sinf(2 * ((float)M_PI) * i/(float)len));
        }
#elif defined(GENERATE_RAMP)
        int len = sizeof(rx_buf) / sizeof(rx_buf[0]);
        for(int i = 0; i < len; i++)
        {
            rx_buf[i] = (i - len/2) * (INT32_MAX / (len/2));
        }
#endif

        process_samples(rx_buf, sizeof(rx_buf) / sizeof(rx_buf[0]));

        i2s_write(static_cast<i2s_port_t>(I2S_NUM), rx_buf, sizeof(rx_buf), &tx_rx_size, 100/portTICK_PERIOD_MS);
        if(tx_rx_size != sizeof(rx_buf))
        {
            ESP_LOGE(TAG, "Sent the wrong number of bytes %d/%d", tx_rx_size, sizeof(rx_buf));
        }
    }
}

/**
 * Set up I2S
 * 
 * Master mode, MCK, BCK, FS controlled using the defines above. Clock is
 * output on GPIO0 (CLK_OUT1) to keep UART0 free.
 */
esp_err_t i2s_setup(gpio_num_t profiling_gpio, shrapnel::AudioParametersBase *audio_param)
{
    esp_err_t err;

    g_profiling_gpio = profiling_gpio;
    /* configure the GPIO used for profiling. It will go high when samples are
     * being processed and return to low once the processing is finished. */
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
        ESP_LOGE(TAG, "gpio_set_level failed %d, %s", err, esp_err_to_name(err));
        return err;
    }

    err = process_init(audio_param);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "process_init failed %d, %s", err, esp_err_to_name(err));
        return err;
    }

    i2s_config_t i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_SIZE,
        .use_apll = true,
        .tx_desc_auto_clear = true, //this will clear the tx buffer when we overrun
        .fixed_mclk = MCK,
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_DI_IO
    };

    //the driver will allocate the queue for use, it just needs to be non-NULL
    //when we pass it in
    i2s_queue = (QueueHandle_t) 42;
    err = i2s_driver_install(static_cast<i2s_port_t>(I2S_NUM), &i2s_config, I2S_QUEUE_SIZE, &i2s_queue);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_driver_install failed %d, %s", err, esp_err_to_name(err));
        return err;
    }
    ESP_LOGD(TAG, "queue address: %p", i2s_queue);

    err = i2s_set_pin(static_cast<i2s_port_t>(I2S_NUM), &pin_config);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_set_pin failed %d, %s", err, esp_err_to_name(err));
        return err;
    }


    /* Get the DMA buffers into their initial states
     *
     * The RX buffers should be all empty, and the TX buffers should all be
     * full.
     */
    err = i2s_stop(static_cast<i2s_port_t>(I2S_NUM));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_stop failed %d, %s", err, esp_err_to_name(err));
        return err;
    }
    size_t read_count;
    do {
        err = i2s_read(static_cast<i2s_port_t>(I2S_NUM), rx_buf, DMA_BUF_SIZE, &read_count, 100);
        if(err != ESP_OK)
        {
            ESP_LOGE(TAG, "i2s_read failed %d, %s", err, esp_err_to_name(err));
            return err;
        }
        ESP_LOGI(TAG, "discarded %d samples from RX buffer", read_count);
    }
    while(read_count != 0);

    err = i2s_zero_dma_buffer(static_cast<i2s_port_t>(I2S_NUM));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_zero_dma_buffer failed %d, %s", err, esp_err_to_name(err));
        return err;
    }

    err = i2s_start(static_cast<i2s_port_t>(I2S_NUM));
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_start failed %d, %s", err, esp_err_to_name(err));
        return err;
    }

    /* Route the I2S master clock to CLK_OUT1/GPIO0 */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
#if I2S_NUM == 0
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL) & 0xFFFFFFF0);
#else
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL) | 0x0000000F);
#endif

    int ret = xTaskCreate(i2s_processing_task, "i2s proc", TASK_STACK, NULL, TASK_PRIO, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Processing task create failed %d", ret);
    }

    ret = xTaskCreate(event_task, "i2s event", TASK_STACK, NULL, TASK_PRIO - 1, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Event task create failed %d", ret);
    }
    
    return ESP_OK;
}
