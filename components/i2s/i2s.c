#include "driver/i2s.h"
#include "i2s.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>//for roundf

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define BITS            (24)
#define MCK             (384*SAMPLE_RATE)

#define SAMPLE_RATE     (48000)
#define I2S_NUM         (1)
#define I2S_BCK_IO      (GPIO_NUM_2)
#define I2S_WS_IO       (GPIO_NUM_4)
#define I2S_DO_IO       (GPIO_NUM_5)
#define I2S_DI_IO       (GPIO_NUM_19)

#define DMA_BUF_SIZE    (960)
#define DMA_BUF_COUNT   (6)

#define TASK_STACK      (3000)
#define TASK_PRIO       (5)

#define I2S_QUEUE_SIZE (4*DMA_BUF_COUNT)
static QueueHandle_t i2s_queue;

#define TAG "codec_i2s"

#if BITS == 24
static int32_t rx_buf[DMA_BUF_SIZE];
#define SAMPLE_MAX INT32_MAX
#elif BITS == 16
static int16_t rx_buf[DMA_BUF_SIZE];
#define SAMPLE_MAX INT16_MAX
#else
#error "Unsupported I2S bit width"
#endif

int att = 0;

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
        i2s_read(I2S_NUM, rx_buf, DMA_BUF_SIZE*4, &tx_rx_size, 100/portTICK_PERIOD_MS);
        if(tx_rx_size != DMA_BUF_SIZE*4)
        {
            ESP_LOGE(TAG, "Got the wrong number of bytes %d/%d", tx_rx_size, DMA_BUF_SIZE*4);
        }

        //TODO do processing here
        for(int i = 0; i < DMA_BUF_SIZE; i++)
        {
            rx_buf[i] /= att;
        }

        i2s_write(I2S_NUM, rx_buf, DMA_BUF_SIZE*4, &tx_rx_size, 100/portTICK_PERIOD_MS);
        if(tx_rx_size != DMA_BUF_SIZE*4)
        {
            ESP_LOGE(TAG, "Sent the wrong number of bytes %d/%d", tx_rx_size, DMA_BUF_SIZE*4);
        }
    }
}

static void i2s_gain_task(void *param)
{
    bool b = 0;
    while(1)
    {
        if(b)
        {
            att = 2;
        }
        else
        {
            att = 1;
        }
        ESP_LOGI(TAG, "amplitude divider set to: %d", att);

        b = !b;
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

/**
 * Set up I2S
 * 
 * Master mode, MCK, BCK, FS controlled using the defines above. Clock is
 * output on GPIO0 (CLK_OUT1) to keep UART0 free.
 */
esp_err_t i2s_setup(void)
{
    esp_err_t err;

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .fixed_mclk = MCK,
        .bits_per_sample = BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_SIZE,
        .use_apll = true,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .tx_desc_auto_clear = true //this will clear the tx buffer when we overrun
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
    err = i2s_driver_install(I2S_NUM, &i2s_config, I2S_QUEUE_SIZE, &i2s_queue);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_driver_install failed %d, %s", err, esp_err_to_name(err));
        return err;
    }
    ESP_LOGD(TAG, "queue address: %p", i2s_queue);

    err = i2s_set_pin(I2S_NUM, &pin_config);
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
    err = i2s_stop(I2S_NUM);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_stop failed %d, %s", err, esp_err_to_name(err));
        return err;
    }
    size_t read_count;
    do {
        err = i2s_read(I2S_NUM, rx_buf, DMA_BUF_SIZE, &read_count, 100);
        if(err != ESP_OK)
        {
            ESP_LOGE(TAG, "i2s_read failed %d, %s", err, esp_err_to_name(err));
            return err;
        }
        ESP_LOGI(TAG, "discarded %d samples from RX buffer", read_count);
    }
    while(read_count != 0);

    err = i2s_zero_dma_buffer(I2S_NUM);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2s_zero_dma_buffer failed %d, %s", err, esp_err_to_name(err));
        return err;
    }

    err = i2s_start(I2S_NUM);
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

    ret = xTaskCreate(i2s_gain_task, "i2s gain", TASK_STACK, NULL, TASK_PRIO - 1, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Gain task create failed %d", ret);
    }

    ret = xTaskCreate(event_task, "i2s event", TASK_STACK, NULL, TASK_PRIO - 1, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Event task create failed %d", ret);
    }
    
    return ESP_OK;
}
