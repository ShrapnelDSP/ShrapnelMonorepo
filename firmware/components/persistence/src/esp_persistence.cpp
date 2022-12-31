#include "esp_persistence.h"
#include "nvs.h"
#include "esp_log.h"

#define STORAGE_NAMESPACE "persistence"
#define TAG "persistence"

namespace shrapnel::persistence {

int EspStorage::save(const char *key, etl::string_view data) {
    ESP_LOGE("DEBUG", "save %s %.*s", key, data.size(), data.data());
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    if (err != ESP_OK) goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_set_blob(nvs_handle, truncated_key.data(), data.data(), data.size()));
    if (err != ESP_OK) goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if (err != ESP_OK) goto out;

    rc = 0;

out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::load(const char *key, etl::istring &data) {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    std::size_t required_size = 0;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle));
    if (err != ESP_OK) goto out;

    // query the required size
    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_get_blob(nvs_handle, truncated_key.data(), nullptr, &required_size));
    if (err != ESP_OK) goto out;

    data.initialize_free_space();
    data.uninitialized_resize(required_size);

    if(data.size() != required_size)
    {
        ESP_LOGE(TAG, "string is too small, need %zu, got %zu", required_size, data.size());
        return -1;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_get_blob(nvs_handle, truncated_key.data(), data.data(), &required_size));
    if (err != ESP_OK) goto out;

    rc = 0;
    ESP_LOGE("DEBUG", "loaded %s %.*s", key, data.size(), data.data());
out:
    nvs_close(nvs_handle);
    return rc;
}

}