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

#include <mutex>
#include <stdio.h>
#include <math.h>
#include <type_traits>

#include "cmd_handling_api.h"
#include "esp_heap_caps.h"
#include "midi_mapping.h"
#include "rapidjson/writer.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "freertos/projdefs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#include "esp_http_server.h"
#include "hardware.h"
#include "i2s.h"
#include "midi_protocol.h"
#include "midi_mapping_json_parser.h"
#include "midi_mapping_json_builder.h"
#include "midi_uart.h"
#include "esp_persistence.h"
#include "pcm3060.h"
#include "profiling.h"
#include "wifi_provisioning.h"
#include "midi_mapping_api.h"
#include "queue.h"

#define TAG "main"
#define QUEUE_LEN 4
#define MAX_CLIENTS 3
#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

using ApiMessage = std::variant<shrapnel::parameters::ApiMessage, shrapnel::midi::MappingApiMessage, shrapnel::events::ApiMessage>;
using FileDescriptor = std::optional<int>;
using AppMessage = std::pair<ApiMessage, FileDescriptor>;

extern "C" void audio_event_send_callback(const AppMessage &message);

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

class ParameterUpdateNotifier {
    public:
    int update(const parameters::id_t &param, float value);
};

class EventSend final {
    public:
    void send(parameters::ApiMessage message, int fd)
    {
        if(fd >= 0) {
            audio_event_send_callback({message, fd});
        } else {
            audio_event_send_callback({message, std::nullopt});
        }
    }
};

template<std::size_t MAX_PARAMETERS>
class ParameterObserver final : public parameters::ParameterObserver {
public:
    explicit ParameterObserver(persistence::Storage &persistence) : persistence{persistence} {}

    void notification(std::pair<const parameters::id_t &, float> parameter) override
    {
        ESP_LOGI(TAG, "notified about parameter change %s %f", parameter.first.data(), parameter.second);
        if (!updated_parameters.available())
        {
            ESP_LOGE(TAG, "no space available");
            return;
        }

        updated_parameters[parameter.first] = parameter.second;

        // TODO throttle this so that parameters are saved every minute or so
        persist_parameters();
    }

private:
    void persist_parameters() {
        for (const auto &param : updated_parameters)
        {
            rapidjson::Document document;

            auto parameter_data = midi::to_json(document, param.second);
            document.Swap(parameter_data);

            rapidjson::StringBuffer buffer;
            rapidjson::Writer writer{buffer};
            document.Accept(writer);

            persistence.save(param.first.data(), buffer.GetString());
        }

        updated_parameters.clear();
    };

    persistence::Storage &persistence;
    etl::map<parameters::id_t, float, MAX_PARAMETERS> updated_parameters;
};

template <typename MappingManagerT>
class MidiMappingObserver final : public midi::MappingObserver {
public:
    explicit MidiMappingObserver(persistence::Storage &persistence,
                                 const MappingManagerT &mapping_manager)
        : persistence{persistence}, mapping_manager{mapping_manager} {}

    void notification(const midi::Mapping::id_t &) override {
        ESP_LOGI(TAG, "Midi mapping has changed");
        persist();
    }

private:
    void persist() {
        rapidjson::Document document;

        auto mapping_data = midi::to_json(document, *mapping_manager.get());
        document.Swap(mapping_data);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer writer{buffer};
        document.Accept(writer);

        persistence.save("midi_mappings", buffer.GetString());
    };

    persistence::Storage &persistence;
    const MappingManagerT &mapping_manager;
};

constexpr const size_t MAX_PARAMETERS = 17;
using AudioParameters = parameters::AudioParameters<MAX_PARAMETERS, 1>;

static Queue<AppMessage, QUEUE_LEN> *in_queue;
static std::shared_ptr<AudioParameters> audio_params;
static parameters::CommandHandling<AudioParameters, EventSend> *cmd_handling;
static midi::MappingManager<ParameterUpdateNotifier, 10, 1>
    *midi_mapping_manager;
static EventSend event_send{};

static Queue<AppMessage, QUEUE_LEN> *out_queue;

/*
 * TODO espressif's http server drops some calls to the work function when
 * there are many calls queued at once. Waiting for the previous execution to
 * finish seems to help, but is probably not a real solution. There will be
 * some internal functions writing to the control socket which could still
 * reproduce the bug.
 */
static SemaphoreHandle_t work_semaphore;

static std::mutex midi_mutex;

static httpd_handle_t _server = nullptr;

extern "C" {

static void websocket_send(void *arg);
static esp_err_t websocket_get_handler(httpd_req_t *req);
static esp_err_t ui_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver(void);
static void stop_webserver(httpd_handle_t server);
static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
static void start_mdns(void);
static void i2c_setup(void);
static void do_nothing(TimerHandle_t) {};

}

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    /* Largest incoming message is a MidiMap::create::response which has a maximum size about 200 bytes */
    char json[256] = {0};

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = reinterpret_cast<uint8_t*>(json),
        .len = 0,
    };

    if(req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Got websocket upgrade request");
        return ESP_OK;
    }

    esp_err_t rc = httpd_ws_recv_frame(req, &pkt, sizeof(json));
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "websocket parse failed");
        return rc;
    }

    /* We should never see any of these packets */
    assert(pkt.type != HTTPD_WS_TYPE_CONTINUE);
    assert(pkt.type != HTTPD_WS_TYPE_BINARY);
    assert(pkt.type != HTTPD_WS_TYPE_CLOSE);
    assert(pkt.type != HTTPD_WS_TYPE_PING);
    assert(pkt.type != HTTPD_WS_TYPE_PONG);
    assert(pkt.final);
    assert(!pkt.fragmented);
    assert(pkt.len <= sizeof(json));

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, pkt.len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, json, sizeof(json), ESP_LOG_VERBOSE);

    int fd = httpd_req_to_sockfd(req);

    rapidjson::Document document;
    document.Parse(json);
    if(document.HasParseError())
    {
        ESP_LOGE(TAG, "Failed to parse incoming JSON");
        return ESP_FAIL;
    }

    {
        auto message = parameters::from_json<parameters::ApiMessage>(document.GetObject());
        if(message.has_value())
        {
            auto out = AppMessage{*message, fd};
            in_queue->send(&out, pdMS_TO_TICKS(100));
            goto out;
        }
    }

    {
        auto message = midi::from_json<midi::MappingApiMessage>(document.GetObject());
        if(message.has_value())
        {
            auto out = AppMessage{*message, fd};
            in_queue->send(&out, pdMS_TO_TICKS(100));

            etl::string<256> buffer;
            etl::string_stream stream{buffer};
            stream << *message;
            ESP_LOGI(TAG, "decoded %s", buffer.data());

            goto out;

        }
    }

out:
    ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));
    return ESP_OK;
}

static esp_err_t ui_get_handler(httpd_req_t *req)
{
    extern const unsigned char html_start[] asm("_binary_index_html_start");
    extern const unsigned char html_end[] asm("_binary_index_html_end");
    const int html_size = (html_end - html_start);

    httpd_resp_send(req, (const char *)html_start, html_size);
    httpd_resp_send(req, nullptr, 0);

    return ESP_OK;
}

static const httpd_uri_t websocket = {
    .uri       = "/websocket",
    .method    = HTTP_GET,
    .handler   = websocket_get_handler,
    .user_ctx  = nullptr,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

static const httpd_uri_t ui = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = ui_get_handler,
    .user_ctx  = nullptr,
    .is_websocket = false,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.ctrl_port = 8081;
    config.max_open_sockets = MAX_CLIENTS;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &websocket);
        httpd_register_uri_handler(server, &ui);
        return server;
    }

    ESP_LOGE(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    /* TODO need to stop tasks that use the web server first */

    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = nullptr;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == nullptr) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

static void wifi_start_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)event_data;
    (void)arg;
    assert(event_base == WIFI_EVENT);
    assert(event_id == WIFI_EVENT_STA_START);

    int rc = esp_wifi_connect();
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "wifi connect failed %d", rc);
    }
}

static void websocket_send(void *arg)
{
    httpd_handle_t server = arg;

    AppMessage message;
    int rc = out_queue->receive(&message, 0);
    if(!rc)
    {
        ESP_LOGE(TAG, "%s failed to receive from queue", __FUNCTION__);
        return;
    }

    if(!message.second.has_value()) {
        ESP_LOGD(TAG, "%s source fd is null", __FUNCTION__);
    } else {
        ESP_LOGD(TAG, "%s source fd = %d", __FUNCTION__, *message.second);
    }

    rapidjson::Document document;

    auto json = std::visit([&](const auto &message) -> rapidjson::Value {
        return to_json(document, message);
    }, message.first);

    rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> buffer;
    rapidjson::Writer writer{buffer};
    json.Swap(document);
    document.Accept(writer);

    const char *payload = buffer.GetString();
    std::size_t payload_len = strlen(payload);

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, payload_len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, payload, payload_len, ESP_LOG_VERBOSE);

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
         // XXX: casting away const, but esp-idf does not modify the buffer
        .payload = reinterpret_cast<uint8_t *>(const_cast<char *>(payload)),
        .len = payload_len,
    };

    int client_fds[MAX_CLIENTS];
    size_t number_of_clients = ARRAY_LENGTH(client_fds);

    httpd_get_client_list((httpd_handle_t) arg, &number_of_clients, client_fds);

    ESP_LOGD(TAG, "n = %zd", number_of_clients);

    assert(number_of_clients <= MAX_CLIENTS);

    for(size_t i = 0; i < number_of_clients; i++)
    {
        int fd = client_fds[i];

        ESP_LOGD(TAG, "fd = %d", fd);

        if(HTTPD_WS_CLIENT_WEBSOCKET != httpd_ws_get_fd_info(server, fd))
        {
            continue;
        }

        if(message.second.has_value() && fd == *message.second)
        {
            continue;
        }

        rc = httpd_ws_send_frame_async(server, fd, &pkt);
        if(ESP_OK != rc)
        {
            ESP_LOGE(TAG, "failed to send to fd %d rc %d", fd, rc);
        }
    }

    xSemaphoreGive(work_semaphore);
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

int ParameterUpdateNotifier::update(const parameters::id_t &param, float value)
{
    auto message = AppMessage{
        parameters::Update{
            param,
            value,
        },
        std::nullopt,
    };
    audio_event_send_callback(message);
    return audio_params->update(param, value);
}

void nvs_debug_print();

extern "C" void app_main(void)
{

  {
    std::binary_semaphore sem{0};

    ESP_LOGI(TAG, "sem max: %d", sem.max());
    sem.release(5);

    while(sem.try_acquire())
    {
      ESP_LOGI(TAG, "acquire");
    }
  }
    ESP_ERROR_CHECK(heap_caps_register_failed_alloc_callback(failed_alloc_callback));

    ESP_ERROR_CHECK(nvs_flash_init());
    nvs_debug_print();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    auto persistence = persistence::EspStorage{};

    work_semaphore = xSemaphoreCreateBinary();
    assert(work_semaphore);
    xSemaphoreGive(work_semaphore);

    in_queue = new Queue<AppMessage, QUEUE_LEN>();
    assert(in_queue);

    out_queue = new Queue<AppMessage, QUEUE_LEN>();
    assert(out_queue);

    audio_params = std::make_shared<AudioParameters>();

    auto create_and_load_parameter = [&](
            const parameters::id_t &name,
            float minimum,
            float maximum,
            float default_value){
        std::optional<float> loaded_value;
        etl::string<128> json_string{};
        int rc = persistence.load(name.data(), json_string);
        if(rc != 0)
        {
            goto out;
        }

        {
            rapidjson::Document document;
            document.Parse(json_string.data());

            if(!document.HasParseError()) {
                loaded_value = midi::from_json<float>(document);
            } else {
                ESP_LOGE(TAG, "document failed to parse '%s'", json_string.data());
            }
        }

out:
        audio_params->create_and_add_parameter(name, minimum, maximum, loaded_value.value_or(default_value));
    };

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
    create_and_load_parameter("chorusBypass", 0, 1, 0);

    ParameterObserver<MAX_PARAMETERS> parameter_observer{persistence};
    audio_params->add_observer(parameter_observer);

    auto parameter_notifier = std::make_shared<ParameterUpdateNotifier>();

    [&] {
        /* TODO How to reduce the memory usage?
         * - We could store each entry in the table at a different key
         * - Use the streaming API so that the entire message does not have to
         *   be parsed at once?
         * - Replace etl::map with more efficient implementation
         */
        etl::string<1500> mapping_json;
        persistence.load("midi_mappings", mapping_json);
        ESP_LOGI(TAG, "saved mappings: %s", mapping_json.c_str());

        rapidjson::Document document;
        document.Parse(mapping_json.c_str());
        auto saved_mappings = midi::from_json<etl::map<midi::Mapping::id_t,
midi::Mapping, 10>>(document);

        midi_mapping_manager =
            saved_mappings.has_value()
                ? new midi::MappingManager<ParameterUpdateNotifier, 10, 1>(
                      parameter_notifier, *saved_mappings)
                : new midi::MappingManager<ParameterUpdateNotifier, 10, 1>(
                      parameter_notifier);

        auto rc = midi_mapping_manager->create(
            {midi::Mapping::id_t{0},
             midi::Mapping{.midi_channel = 1,
                           .cc_number = 7,
                           .parameter_name = "tubeScreamerTone"}});
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to create initial mapping");
        }
    }();

    MidiMappingObserver mapping_observer{persistence, *midi_mapping_manager};
    midi_mapping_manager->add_observer(mapping_observer);

    cmd_handling = new parameters::CommandHandling<AudioParameters, EventSend>(
            audio_params.get(),
            event_send);

    i2c_setup();

    profiling_mutex = xSemaphoreCreateMutex();
    i2s_setup(PROFILING_GPIO, audio_params.get());

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
    rc = xTaskCreate(i2s_profiling_task, "i2s profiling", 2000, NULL, tskIDLE_PRIORITY + 2, NULL);
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Profiling task create failed %d", rc);
    }
#endif

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Start the mdns service */
    start_mdns();

    {
        wifi_provisioning::WiFiProvisioning wifi_provisioning{};

#if SHRAPNEL_RESET_WIFI_CREDENTIALS
        ESP_LOGW(TAG, "Reseting wifi provisioning");
        wifi_prov_mgr_reset_provisioning();
#endif

        if(!wifi_provisioning.is_provisioned())
        {
             wifi_provisioning.wait_for_provisioning();
             /* start the websocket server */
             connect_handler(&_server, IP_EVENT, IP_EVENT_STA_GOT_IP, nullptr);
        }
    }

    /* TODO Need to enter provisioning mode again if the WiFi credentials are
     *      not working. Probably the AP passphrase was changed since we got
     *      provisionined.
     */

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
    ESP_ERROR_CHECK(esp_event_handler_register(
                IP_EVENT,
                IP_EVENT_STA_GOT_IP,
                connect_handler,
                &_server));
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_EVENT,
                WIFI_EVENT_STA_DISCONNECTED,
                disconnect_handler,
                &_server));
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_EVENT,
                WIFI_EVENT_STA_START,
                wifi_start_handler,
                nullptr));

    /* Start Wi-Fi station */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "setup done");
    ESP_LOGI(TAG, "stack: %d", uxTaskGetStackHighWaterMark(NULL));
    heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);

    TimerHandle_t clipping_throttle_timer = xTimerCreate(
        "clipping throttle",
        pdMS_TO_TICKS(1000),
        false,
        nullptr,
        do_nothing);

    auto midi_uart = new midi::EspMidiUart(UART_NUM_MIDI, GPIO_NUM_MIDI);

    auto on_midi_message = [] (midi::Message message) {
        etl::string<40> buffer;
        etl::string_stream stream{buffer};
        stream << message;
        ESP_LOGI(TAG, "%s", buffer.data());

        {
            std::scoped_lock lock{midi_mutex};
            midi_mapping_manager->process_message(message);
        }
    };

    auto midi_decoder = new midi::Decoder(on_midi_message);

#if configUSE_TRACE_FACILITY && configUSE_STATS_FORMATTING_FUNCTIONS
    constexpr size_t characters_per_task = 40;
    constexpr size_t approximate_task_count = 20;
    char buffer[characters_per_task * approximate_task_count + 1] = {0};
    vTaskList(buffer);
    // crash if the buffer was overflowed
    assert(buffer[sizeof(buffer) - 1] == '\0');
    ESP_LOGI(TAG, "Task list:\n%s", buffer);
#endif

    events::input_clipped.test_and_set();
    events::output_clipped.test_and_set();

    while(1)
    {
        if(auto byte = midi_uart->get_byte(pdMS_TO_TICKS(10)); byte.has_value())
        {
            ESP_LOGI(TAG, "midi got byte 0x%02x", *byte);

            midi_decoder->decode(*byte);
        }

        if(AppMessage message; in_queue->receive(&message, 0))
        {
            auto fd = message.second;

            std::visit([&](const auto &message){
                using T = std::decay_t<decltype(message)>;

                if constexpr(std::is_same_v<T, parameters::ApiMessage>) {
                    if(!fd.has_value())
                    {
                        ESP_LOGE(TAG, "Must always have fd");
                    }

                    cmd_handling->dispatch(message, *fd);
                } else if constexpr(std::is_same_v<T, midi::MappingApiMessage>) {
                    std::scoped_lock lock{midi_mutex};

                    auto response = std::visit([&](const auto &message) -> std::optional<midi::MappingApiMessage> {
                        using T = std::decay_t<decltype(message)>;

                        if constexpr (std::is_same_v<T, midi::GetRequest>) {
                            auto mappings = midi_mapping_manager->get();
                            return midi::GetResponse{mappings};
                        } else if constexpr (std::is_same_v<T, midi::CreateRequest>) {
                            auto rc = midi_mapping_manager->create(message.mapping);
                            if(rc == 0)
                            {
                                return midi::CreateResponse{message.mapping};
                            }
                        } else if constexpr (std::is_same_v<T, midi::Update>) {
                            // return code ignored, as there is no way to indicate
                            // the error to the frontend
                            (void)midi_mapping_manager->update(message.mapping);
                        } else if constexpr (std::is_same_v<T, midi::Remove>) {
                            midi_mapping_manager->remove(message.id);
                        } else {
                            ESP_LOGE(TAG, "Unhandled message type");
                        }

                        return std::nullopt;
                    }, message);

                    if(response.has_value())
                    {
                        audio_event_send_callback({*response, std::nullopt});
                    }
                } else if constexpr(std::is_same_v<T, events::ApiMessage>) {
                    audio_event_send_callback({message, std::nullopt});
                }
            }, message.first);
        }

        {
            // i2s produces an event on each buffer TX/RX. We process all the
            // events in the current iteration, so that the queue doesn't fill
            // up.
            i2s_event_t event;
            while(xQueueReceive(i2s_queue, &event, 0))
            {
                log_i2s_event(event);
            }
        }

        if(!xTimerIsTimerActive(clipping_throttle_timer))
        {
            if(!events::input_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "input was clipped");
                audio_event_send_callback({events::InputClipped{}, std::nullopt});
                xTimerReset(clipping_throttle_timer, pdMS_TO_TICKS(10));
            }

            if(!events::output_clipped.test_and_set())
            {
                ESP_LOGI(TAG, "output was clipped");
                audio_event_send_callback({events::OutputClipped{}, std::nullopt});
                xTimerReset(clipping_throttle_timer, pdMS_TO_TICKS(10));
            }
        }
    }
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

extern "C" void audio_event_send_callback(const AppMessage &message)
{
    ESP_LOGD(TAG, "%s %s", __FUNCTION__, pcTaskGetName(NULL));

    if(errQUEUE_FULL ==
            shrapnel::out_queue->send(&message, pdMS_TO_TICKS(100)))
    {
        ESP_LOGE(TAG, "Failed to send message to websocket");
        return;
    }

    if(!xSemaphoreTake(shrapnel::work_semaphore, 1000 / portTICK_PERIOD_MS))
    {
        ESP_LOGE(TAG, "work semaphore timed out");
        return;
    }

    esp_err_t rc = httpd_queue_work(
            shrapnel::_server,
            shrapnel::websocket_send,
            shrapnel::_server);
    if(ESP_OK != rc)
    {
        ESP_LOGE(TAG, "failed to queue work for server");
    }
}
