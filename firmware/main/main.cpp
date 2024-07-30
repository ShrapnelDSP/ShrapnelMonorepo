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

#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "os/queue.h"
#include "wifi_provisioning/manager.h"
#include <atomic>
#include <etl/state_chart.h>
#include <math.h>
#include <mutex>
#include <stdio.h>
#include <type_traits>

#include "cmd_handling_api.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/projdefs.h"
#include "midi_mapping.h"

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_debug_helpers.h"
#include "esp_netif.h"
#include "mdns.h"
#include "nvs_flash.h"
#include <assert.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "audio_events.h"
#include "audio_param.h"
#include "cmd_handling.h"
#include "esp_crud.h"
#include "esp_midi_uart.h"
#include "esp_persistence.h"
#include "hardware.h"
#include "i2s.h"
#include "main_thread.h"
#include "messages.h"
#include "midi_mapping_api.h"
#include "midi_protocol.h"
#include "midi_uart.h"
#include "os/debug.h"
#include "os/queue.h"
#include "os/timer.h"
#include "pcm3060.h"
#include "profiling.h"
#include "server.h"
#include "shrapnel_console.h"
#include "wifi_state_machine.h"

#define TAG "main"
#define QUEUE_LEN 4
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

using WifiQueue = shrapnel::Queue<shrapnel::wifi::InternalEvent, 3>;

namespace shrapnel {

using Crud = persistence::EspCrud<256>;

template <std::size_t MAX_PARAMETERS>
class ParameterObserver final : public parameters::ParameterObserver
{
public:
    explicit ParameterObserver(
        std::shared_ptr<persistence::Storage> a_persistence)
        : is_save_throttled{true},
          persistence{std::move(a_persistence)},
          timer{"param save throttle",
                os::ms_to_ticks(10'000),
                false,
                etl::delegate<void(void)>::create<
                    ParameterObserver<MAX_PARAMETERS>,
                    &ParameterObserver<MAX_PARAMETERS>::timer_callback>(*this)}
    {
    }

    void
    notification(std::pair<const parameters::id_t &, float> parameter) override
    {
        auto &[id, value] = parameter;
        ESP_LOGI(
            TAG, "notified about parameter change %s %f", id.data(), value);
        if(!updated_parameters.available())
        {
            ESP_LOGE(TAG, "no space available");
            return;
        }

        updated_parameters[id] = value;

        if(!timer.is_active())
        {
            if(os::timer_error::TIMER_START_SUCCESS !=
               timer.start(os::ms_to_ticks(5)))
            {
                ESP_LOGE(TAG, "Failed to start parameter observer timer");
            }
        }
    }

    void loop()
    {
        if(!is_save_throttled.test_and_set())
        {
            persist_parameters();
            ESP_LOGI(TAG, "Parameters saved to NVS");
        }
    }

private:
    void timer_callback()
    {
        is_save_throttled.clear();
        is_save_throttled.notify_all();
    }

    void persist_parameters()
    {
        for(const auto &param : updated_parameters)
        {
            persistence->save(param.first.data(), param.second);
        }

        updated_parameters.clear();
    }

    std::atomic_flag is_save_throttled;
    std::shared_ptr<persistence::Storage> persistence;
    os::Timer timer;
    etl::map<parameters::id_t, float, MAX_PARAMETERS> updated_parameters;
};

extern "C" {

static void disconnect_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data);
static void connect_handler(void *arg,
                            esp_event_base_t event_base,
                            int32_t event_id,
                            void *event_data);
static void start_mdns(void);
static void i2c_setup(void);
}

static void disconnect_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    ESP_LOGI(TAG, "WiFi disconnected");
    auto queue{reinterpret_cast<WifiQueue *>(arg)};
    wifi::InternalEvent event{wifi::InternalEvent::DISCONNECT};
    auto rc{queue->send(&event, 0)};
    if(rc != queue_error::SUCCESS)
    {
        ESP_LOGE(TAG, "Failed to send disconnect event to queue");
    }
}

static void connect_handler(void *arg,
                            esp_event_base_t event_base,
                            int32_t event_id,
                            void *event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    ESP_LOGI(TAG, "WiFi connected");
    auto queue{reinterpret_cast<WifiQueue *>(arg)};
    wifi::InternalEvent event{wifi::InternalEvent::CONNECT_SUCCESS};
    auto rc{queue->send(&event, 0)};
    if(rc != queue_error::SUCCESS)
    {
        ESP_LOGE(TAG, "Failed to send connect event to queue");
    }
}

static void wifi_start_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    (void)event_data;
    assert(event_base == WIFI_EVENT);
    assert(event_id == WIFI_EVENT_STA_START);

    auto queue{reinterpret_cast<WifiQueue *>(arg)};
    wifi::InternalEvent event{wifi::InternalEvent::STARTED};
    auto rc{queue->send(&event, 0)};
    if(rc != queue_error::SUCCESS)
    {
        ESP_LOGE(TAG, "Failed to send start event to queue");
    }
}

static void start_mdns(void)
{
    esp_err_t err = mdns_init();
    if(err)
    {
        ESP_LOGE(TAG, "MDNS Init failed: %d %s", err, esp_err_to_name(err));
        return;
    }

    err = mdns_hostname_set("guitar-dsp");
    if(err)
    {
        ESP_LOGE(TAG,
                 "MDNS failed to set host name %d %s",
                 err,
                 esp_err_to_name(err));
        return;
    }
    err = mdns_instance_name_set("Barabas' Guitar Processor");
    if(err)
    {
        ESP_LOGE(TAG,
                 "MDNS failed to set instance name %d %s",
                 err,
                 esp_err_to_name(err));
        return;
    }

    err = mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    if(err)
    {
        ESP_LOGE(
            TAG, "MDNS failed to add service %d %s", err, esp_err_to_name(err));
        return;
    }
    err = mdns_service_instance_name_set(
        "_http", "_tcp", "Barabas' Guitar Processor Web Server");
    if(err)
    {
        ESP_LOGE(TAG,
                 "MDNS failed to set service name %d %s",
                 err,
                 esp_err_to_name(err));
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
        .master =
            {
                .clk_speed = 100 * 1000,
            },
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0));
}

static void
failed_alloc_callback(size_t size, uint32_t caps, const char *function_name)
{
    ESP_LOGE(TAG,
             "allocation failed. size=%zu caps=%08" PRIx32 "h, function=%s",
             size,
             caps,
             function_name);
    heap_caps_print_heap_info(caps);
    abort();
}

void nvs_debug_print();

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(
        heap_caps_register_failed_alloc_callback(failed_alloc_callback));

    ESP_ERROR_CHECK(nvs_flash_init());
    nvs_debug_print();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    auto persistence = std::make_shared<persistence::EspStorage>();
    auto audio_params = std::make_shared<parameters::AudioParameters<20, 1>>();

    auto create_and_load_parameter = [&](const parameters::id_t &name,
                                         float minimum,
                                         float maximum,
                                         float default_value)
    {
        std::optional<float> loaded_value;
        float value;
        int rc = persistence->load(name.data(), value);
        if(rc != 0)
        {
            ESP_LOGW(TAG, "Parameter %s failed to load", name.data());
            goto out;
        }

        loaded_value = value;
    out:
        auto range = maximum - minimum;
        rc = audio_params->create_and_add_parameter(
            name,
            minimum,
            maximum,
            loaded_value.has_value() ? *loaded_value * range + minimum
                                     : default_value);
        if(rc != 0)
        {
            ESP_LOGE(TAG, "Failed to create parameter %s", name.c_str());
        }
    };

    // XXX: These are duplicated in the JUCE plugin, be sure to update both at
    // the same time
    create_and_load_parameter("ampGain", 0, 1, 0.5);
    create_and_load_parameter("ampChannel", 0, 1, 0);
    create_and_load_parameter("bass", 0, 1, 0.5);
    create_and_load_parameter("middle", 0, 1, 0.5);
    create_and_load_parameter("treble", 0, 1, 0.5);
    //contour gets unstable when set to 0
    create_and_load_parameter("contour", 0.01, 1, 0.5);
    create_and_load_parameter("volume", -30, 0, -15);

    create_and_load_parameter("noiseGateThreshold", -80, 0, -60);
    create_and_load_parameter("noiseGateHysteresis", 0, 5, 0);
    create_and_load_parameter("noiseGateAttack", 1, 50, 10);
    create_and_load_parameter("noiseGateHold", 1, 250, 50);
    create_and_load_parameter("noiseGateRelease", 1, 250, 50);
    create_and_load_parameter("noiseGateBypass", 0, 1, 0);

    create_and_load_parameter("chorusRate", 0.1, 4, 0.95);
    create_and_load_parameter("chorusDepth", 0, 1, 0.3);
    create_and_load_parameter("chorusMix", 0, 1, 0.8);
    create_and_load_parameter("chorusBypass", 0, 1, 1);

    create_and_load_parameter("wahPosition", 0, 1, 0.5);
    create_and_load_parameter("wahVocal", 0, 1, 0);
    create_and_load_parameter("wahBypass", 0, 1, 1);

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
    rc = xTaskCreate(profiling_task,
                     "i2s profiling",
                     2000,
                     NULL,
                     tskIDLE_PRIORITY + 2,
                     NULL);
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Profiling task create failed %d", rc);
    }
#endif

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    auto wifi_queue = WifiQueue();

    auto wifi_send_event = [&](wifi::InternalEvent event)
    {
        auto rc = wifi_queue.send(&event, 0);
        if(rc != queue_error::SUCCESS)
        {
            ESP_LOGE(TAG, "Failed to post wifi event to queue");
        }
    };

    auto in_queue = new Queue<AppMessage, QUEUE_LEN>;
    auto out_queue = new Queue<AppMessage, QUEUE_LEN>;
    auto server = new Server(in_queue, out_queue);

    auto app_send_event = [&](wifi::UserEvent event)
    {
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

    static auto wifi = wifi::WifiStateMachine(wifi_send_event, app_send_event);

    auto wifi_state_chart =
        etl::state_chart_ct<wifi::WifiStateMachine,
                            wifi,
                            wifi::WifiStateMachine::transition_table,
                            ARRAY_LENGTH(
                                wifi::WifiStateMachine::transition_table),
                            wifi::WifiStateMachine::state_table,
                            ARRAY_LENGTH(wifi::WifiStateMachine::state_table),
                            wifi::State::INIT>();
    wifi_state_chart.start();

    /* Register event handlers to send events on to wifi state machine */
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, connect_handler, &wifi_queue));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                               WIFI_EVENT_STA_DISCONNECTED,
                                               disconnect_handler,
                                               &wifi_queue));
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, WIFI_EVENT_STA_START, wifi_start_handler, &wifi_queue));

    auto midi_uart = new midi::EspMidiUart(UART_NUM_MIDI, GPIO_NUM_MIDI);

    debug_dump_task_list();

    auto send_message = [&](const AppMessage &message)
    { server->send_message(message); };

    auto main_thread =
        MainThread<QUEUE_LEN, parameters::AudioParameters<20, 1>>(
            send_message,
            *in_queue,
            audio_params,
            persistence,
            std::make_unique<Crud>("nvs", "midi_mapping"),
            std::make_unique<Crud>("nvs", "presets"));

    auto send_midi_message = [&](const midi::Message &message)
    {
        auto app_message = AppMessage{ApiMessage{message}, std::nullopt};
        int rc = in_queue->send(&app_message, portMAX_DELAY);

        if(rc != pdPASS)
        {
            ESP_LOGE(TAG, "Failed to send to main queue %d", rc);
        }
    };

    auto setup_wifi = [&](const char *ssid, const char *password)
    {
        ESP_LOGI(TAG, "wifi config %s %s", ssid, password);

        wifi_config_t wifi_cfg{};
        esp_err_t rc = esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg);
        if(rc != ESP_OK)
        {
            ESP_LOGE(TAG, "get config failed 0x%x %s", rc, esp_err_to_name(rc));
            return;
        }

        // According to Espressif code in the Wi-Fi provisioning component:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
        // SSID can use all 32 bytes safely
        strncpy((char *)wifi_cfg.sta.ssid, ssid, sizeof wifi_cfg.sta.ssid);
#pragma GCC diagnostic pop
        // Password should use up to 63 bytes, and leave a null termination
        strlcpy((char *)wifi_cfg.sta.password,
                password,
                sizeof wifi_cfg.sta.password);

        rc = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
        if(rc != ESP_OK)
        {
            ESP_LOGE(TAG, "set config failed 0x%x %s", rc, esp_err_to_name(rc));
            return;
        }

        wifi_send_event(wifi::InternalEvent::PROVISIONING_BYPASS);
    };

    auto console = shrapnel::Console(send_midi_message, setup_wifi);

    audio::i2s_setup(PROFILING_GPIO, audio_params.get());

    ParameterObserver<20> parameter_observer{persistence};
    audio_params->add_observer(parameter_observer);

    ESP_LOGI(TAG, "setup done");
    ESP_LOGI(TAG, "stack: %d", uxTaskGetStackHighWaterMark(NULL));
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        auto tick_count_start = xTaskGetTickCount();

        main_thread.loop();

        parameter_observer.loop();

        {
            // i2s produces an event on each buffer TX/RX. We process all the
            // events in the current iteration, so that the queue doesn't fill
            // up.
            i2s_event_t event;
            while(xQueueReceive(audio::i2s_queue, &event, 0))
            {
                audio::log_i2s_event(event);
            }
        }

        wifi::InternalEvent wifi_event;
        while(queue_error::SUCCESS == wifi_queue.receive(&wifi_event, 0))
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

        {
            int c = EOF;
            do
            {
                c = fgetc(stdin);
                if(c != EOF)
                {
                    console.handle_character(c);
                }
            } while(c != EOF);
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
            ESP_LOGW(TAG,
                     "slow iteration: %d ms",
                     (int)pdTICKS_TO_MS(tick_count_iteration));
        }
    }
}

void nvs_debug_print()
{
    ESP_LOGI(TAG, "dumping NVS using C interface");
    nvs_iterator_t it = NULL;
    esp_err_t res = nvs_entry_find("nvs", "persistence", NVS_TYPE_ANY, &it);
    while(res == ESP_OK)
    {
        nvs_entry_info_t info;
        nvs_entry_info(
            it,
            &info); // Can omit error check if parameters are guaranteed to be non-NULL
        ESP_LOGI(TAG, "key '%s', type '%d'", info.key, info.type);
        res = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);
}

} // namespace shrapnel