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

#include "esp_persistence.h"
#include "esp_log.h"
#include "nvs.h"

#define STORAGE_NAMESPACE "persistence"
#define TAG "persistence"

namespace shrapnel::persistence {

EspStorage::EspStorage()
{
    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));

    ESP_LOGI(TAG, "Started EspStorage for with handle %" PRIu32, nvs_handle);
}

EspStorage::~EspStorage() { nvs_close(nvs_handle); }

int EspStorage::save(const char *key, std::span<uint8_t> data)
{
    esp_err_t err;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_blob(nvs_handle, key, data.data(), data.size()));
    if(err != ESP_OK)
        return -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        return -1;

    return 0;
}

int EspStorage::save(const char *key, etl::string_view data)
{
    esp_err_t err;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_blob(
            nvs_handle, truncated_key.data(), data.data(), data.size()));
    if(err != ESP_OK)
        return -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        return -1;

    return 0;
}

int EspStorage::save(const char *key, uint32_t data)
{
    esp_err_t err;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_u32(nvs_handle, truncated_key.data(), data));
    if(err != ESP_OK)
        return -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        return -1;

    return 0;
}

int EspStorage::save(const char *key, float data)
{
    esp_err_t err;

    etl::string<15> truncated_key{key};

    uint32_t value;
    memcpy(&value, &data, sizeof value);
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_u32(nvs_handle, truncated_key.data(), value));
    if(err != ESP_OK)
        return -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        return -1;

    return 0;
}

int EspStorage::load(const char *key, std::span<uint8_t> &buffer)
{
    esp_err_t err;
    std::size_t required_size = 0;

    // query the required size
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(nvs_handle, key, nullptr, &required_size));
    if(err != ESP_OK)
        return -1;

    if(required_size > buffer.size())
    {
        ESP_LOGE(TAG,
                 "Not enough space in the output buffer. Need %zu, got %zu",
                 required_size,
                 buffer.size());
        return -1;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(nvs_handle, key, buffer.data(), &required_size));
    if(err != ESP_OK)
        return -1;

    buffer = buffer.subspan(0, required_size);
    return 0;
}

int EspStorage::load(const char *key, etl::istring &data)
{
    esp_err_t err;
    std::size_t required_size = 0;

    etl::string<15> truncated_key{key};

    // query the required size
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(
            nvs_handle, truncated_key.data(), nullptr, &required_size));
    if(err != ESP_OK)
        return -1;

    data.initialize_free_space();
    data.uninitialized_resize(required_size);

    if(data.size() != required_size)
    {
        ESP_LOGE(TAG,
                 "string is too small, need %zu, got %zu",
                 required_size,
                 data.size());
        return -1;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(
            nvs_handle, truncated_key.data(), data.data(), &required_size));
    if(err != ESP_OK)
        return -1;

    return 0;
}

int EspStorage::load(const char *key, uint32_t &data)
{
    esp_err_t err;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_u32(nvs_handle, truncated_key.data(), &data));
    if(err != ESP_OK)
        return -1;

    return 0;
}

int EspStorage::load(const char *key, float &data)
{
    esp_err_t err;

    etl::string<15> truncated_key{key};

    uint32_t value;
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_u32(nvs_handle, truncated_key.data(), &value));
    if(err != ESP_OK)
        return -1;

    memcpy(&data, &value, sizeof value);

    return 0;
}

} // namespace shrapnel::persistence