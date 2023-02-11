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

#include <atomic>
#include <mutex>
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "queue.h"
#include "wifi_provisioning/manager.h"
#include <etl/state_chart.h>
#include <stdio.h>
#include <math.h>
#include <type_traits>

#include "cmd_handling_api.h"
#include "esp_heap_caps.h"
#include "midi_mapping.h"
#include "rapidjson/writer.h"
#include "esp_log.h"
#include "freertos/projdefs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOSConfig.h"

#include <assert.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_debug_helpers.h"
#include "mdns.h"

#include "audio_param.h"
#include "audio_events.h"
#include "cmd_handling.h"
#include "hardware.h"
#include "i2s.h"
#include "main_thread.h"
#include "midi_protocol.h"
#include "midi_mapping_json_parser.h"
#include "messages.h"
#include "midi_mapping_json_builder.h"
#include "midi_uart.h"
#include "esp_persistence.h"
#include "pcm3060.h"
#include "profiling.h"
#include "midi_mapping_api.h"
#include "queue.h"
#include "wifi_state_machine.h"
#include "server.h"
#include "timer.h"

#include "iir_concrete.h"

#define TAG "main"
#define QUEUE_LEN 4
#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

using WifiQueue = shrapnel::Queue<shrapnel::wifi::InternalEvent, 3>;

namespace shrapnel {

// TODO move all the json parser function into a json namespace
namespace midi {
    using midi::from_json;

    template<> std::optional<float> from_json(const rapidjson::Value &value)
    {
        if(!value.IsFloat()) {
            ESP_LOGE(TAG, "not float");
            return std::nullopt;
        }
        return value.GetFloat();
    }

    template<>
    rapidjson::Value to_json(rapidjson::Document &document, const float &object)
    {
        rapidjson::Value out{};
        out.SetFloat(object);
        return out;
    }
}

extern "C" {

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
static void start_mdns(void);
static void i2c_setup(void);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    ESP_LOGI(TAG, "WiFi disconnected");
    auto queue{reinterpret_cast<WifiQueue *>(arg)};
    wifi::InternalEvent event{wifi::InternalEvent::DISCONNECT};
    int rc{queue->send(&event, 0)};
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to send disconnect event to queue");
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    ESP_LOGI(TAG, "WiFi connected");
    auto queue{reinterpret_cast<WifiQueue *>(arg)};
    wifi::InternalEvent event{wifi::InternalEvent::CONNECT_SUCCESS};
    int rc{queue->send(&event, 0)};
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to send connect event to queue");
    }
}

static void wifi_start_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)event_data;
    assert(event_base == WIFI_EVENT);
    assert(event_id == WIFI_EVENT_STA_START);

    auto queue{reinterpret_cast<WifiQueue *>(arg)};
    wifi::InternalEvent event{wifi::InternalEvent::STARTED};
    int rc{queue->send(&event, 0)};
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to send start event to queue");
    }
}

static void start_mdns(void)
{
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGE(TAG, "MDNS Init failed: %d %s", err, esp_err_to_name(err));
        return;
    }

    err = mdns_hostname_set("guitar-dsp");
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to set host name %d %s", err, esp_err_to_name(err));
        return;
    }
    err = mdns_instance_name_set("Barabas' Guitar Processor");
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to set instance name %d %s", err, esp_err_to_name(err));
        return;
    }

    err = mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to add service %d %s", err, esp_err_to_name(err));
        return;
    }
    err = mdns_service_instance_name_set("_http", "_tcp", "Barabas' Guitar Processor Web Server");
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to set service name %d %s", err, esp_err_to_name(err));
        return;
    }
}

static void i2c_setup(void)
{
    const i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_I2C_SDA,
        .scl_io_num = GPIO_I2C_SCL,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .master = {
            .clk_speed = 100 * 1000,
        },
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0));
}

static void failed_alloc_callback(size_t size, uint32_t caps, const char *function_name)
{
    ESP_LOGE(TAG, "allocation failed. size=%zu caps=%08xh, function=%s", size, caps, function_name);
    heap_caps_print_heap_info(caps);
    abort();
}

void nvs_debug_print();
void debug_dump_task_list();

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(heap_caps_register_failed_alloc_callback(failed_alloc_callback));

    ESP_ERROR_CHECK(nvs_flash_init());
    nvs_debug_print();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    auto persistence = persistence::EspStorage{};
    auto audio_params = std::make_shared<AudioParameters>();

    i2c_setup();
    profiling_init(DMA_BUF_SIZE, SAMPLE_RATE);

    //dac must be powered up after the i2s clocks have stabilised
    vTaskDelay(100 / portTICK_PERIOD_MS);

#if HARDWARE == HW_PCB_REV1
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_CODEC_NRESET,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t rc = gpio_set_level(GPIO_CODEC_NRESET, 0);
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_set_level failed %d, %s", rc, esp_err_to_name(rc));
    }

    rc = gpio_config(&io_conf);
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_config failed %d, %s", rc, esp_err_to_name(rc));
    }

    vTaskDelay(5 / portTICK_PERIOD_MS);

    rc = gpio_set_level(GPIO_CODEC_NRESET, 1);
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_set_level failed %d, %s", rc, esp_err_to_name(rc));
    }

    vTaskDelay(5 / portTICK_PERIOD_MS);

#endif

    pcm3060_init(I2C_NUM, 0);
    rc = pcm3060_power_up();
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "pcm3060 power up failed");
    }

    /* Start the mdns service */
    start_mdns();

#if 0
    rc = xTaskCreate(profiling_task, "i2s profiling", 2000, NULL, tskIDLE_PRIORITY + 2, NULL);
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Profiling task create failed %d", rc);
    }
#endif

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    auto wifi_queue = WifiQueue();

    auto wifi_send_event = [&] (wifi::InternalEvent event) {
        auto rc = wifi_queue.send(&event, 0);
        if(rc != pdPASS)
        {
            ESP_LOGE(TAG, "Failed to post wifi event to queue");
        }
    };

    auto in_queue = new Queue<AppMessage, QUEUE_LEN>;
    auto out_queue = new Queue<AppMessage, QUEUE_LEN>;
    auto server = new Server(in_queue, out_queue);

    auto app_send_event = [&] (wifi::UserEvent event) {
        switch(event)
        {
            case wifi::UserEvent::CONNECTED:
                ESP_LOGI(TAG, "Starting webserver");
                server->start();
                break;
            case wifi::UserEvent::DISCONNECTED:
                ESP_LOGI(TAG, "Stopping webserver");
                server->stop();
                break;
            default:
                ESP_LOGE(TAG, "Unhandled event %d", event.get_value());
                break;
        }
    };

    static auto wifi = wifi::WifiStateMachine(
            wifi_send_event,
            app_send_event);

    auto wifi_state_chart = etl::state_chart_ct<
        wifi::WifiStateMachine,
        wifi,
        wifi::WifiStateMachine::transition_table,
        ARRAY_LENGTH(wifi::WifiStateMachine::transition_table),
        wifi::WifiStateMachine::state_table,
        ARRAY_LENGTH(wifi::WifiStateMachine::state_table),
        wifi::State::INIT>();
    wifi_state_chart.start();

    /* Register event handlers to send events on to wifi state machine */
    ESP_ERROR_CHECK(esp_event_handler_register(
                IP_EVENT,
                IP_EVENT_STA_GOT_IP,
                connect_handler,
                &wifi_queue));
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_EVENT,
                WIFI_EVENT_STA_DISCONNECTED,
                disconnect_handler,
                &wifi_queue));
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_EVENT,
                WIFI_EVENT_STA_START,
                wifi_start_handler,
                &wifi_queue));

    auto midi_uart = new midi::EspMidiUart(UART_NUM_MIDI, GPIO_NUM_MIDI);

    debug_dump_task_list();

    auto main_thread =
        MainThread<MAX_PARAMETERS, QUEUE_LEN>(*server,
                                              *in_queue,
                                              midi_uart,
                                              audio_params,
                                              persistence);

    audio::i2s_setup(PROFILING_GPIO, audio_params.get());

    ESP_LOGI(TAG, "setup done");
    ESP_LOGI(TAG, "stack: %d", uxTaskGetStackHighWaterMark(NULL));
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        auto tick_count_start = xTaskGetTickCount();

        main_thread.loop();

        wifi::InternalEvent wifi_event;
        while(pdPASS == wifi_queue.receive(&wifi_event, 0))
        {
            wifi::State state{wifi_state_chart.get_state_id()};
            ESP_LOGI(
                TAG, "state: %s event: %s", state.c_str(), wifi_event.c_str());

            wifi_state_chart.process_event(wifi_event);

            wifi::State new_state{wifi_state_chart.get_state_id()};
            if(new_state != state)
            {
                ESP_LOGI(TAG, "changed to state: %s", new_state.c_str());
            }
        }

        /* Check if the current iteration of the main loop took too long. This
         * might be caused by blocking while handling events.
         *
         * It can also be caused by another thread that has a higher priority
         * than the main thread.
         */
        auto tick_count_iteration = xTaskGetTickCount() - tick_count_start;
        if(tick_count_iteration > pdMS_TO_TICKS(5))
        {
            ESP_LOGW(TAG, "slow iteration: %d ms", pdTICKS_TO_MS(tick_count_iteration));
        }
    }
}

void debug_dump_task_list()
{
#if configUSE_TRACE_FACILITY && configUSE_STATS_FORMATTING_FUNCTIONS
    constexpr size_t characters_per_task = 40;
    constexpr size_t approximate_task_count = 20;
    char buffer[characters_per_task * approximate_task_count + 1] = {0};
    vTaskList(buffer);
    // crash if the buffer was overflowed
    assert(buffer[sizeof(buffer) - 1] == '\0');
    ESP_LOGI(TAG, "Task list:\n%s", buffer);
#endif
}

void nvs_debug_print()
{
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", "persistence", NVS_TYPE_ANY, &it);
    while(res == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
        ESP_LOGI(TAG, "key '%s', type '%d'", info.key, info.type);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);
}

}