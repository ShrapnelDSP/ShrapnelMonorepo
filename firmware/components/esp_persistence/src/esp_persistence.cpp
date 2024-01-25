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

int EspStorage::save(const char *key, std::span<uint8_t> data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_blob(nvs_handle, key, data.data(), data.size()));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        goto out;

    rc = 0;

out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::save(const char *key, etl::string_view data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_blob(
            nvs_handle, truncated_key.data(), data.data(), data.size()));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        goto out;

    rc = 0;

out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::save(const char *key, uint32_t data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_u32(nvs_handle, truncated_key.data(), data));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        goto out;

    rc = 0;

out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::save(const char *key, float data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    uint32_t value;
    memcpy(&value, &data, sizeof value);
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_set_u32(nvs_handle, truncated_key.data(), value));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
    if(err != ESP_OK)
        goto out;

    rc = 0;

out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::load(const char *key, std::span<uint8_t> &buffer)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    std::size_t required_size = 0;
    int rc = -1;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    // query the required size
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(nvs_handle, key, nullptr, &required_size));
    if(err != ESP_OK)
        goto out;

    if(required_size > buffer.size())
    {
        ESP_LOGE(TAG,
                 "Not enough space in the output buffer. Need %zu, got %zu",
                 required_size,
                 buffer.size());
        goto out;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(nvs_handle, key, buffer.data(), &required_size));
    if(err != ESP_OK)
        goto out;

    buffer = buffer.subspan(0, required_size);
    rc = 0;
out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::load(const char *key, etl::istring &data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    std::size_t required_size = 0;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    // query the required size
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_blob(
            nvs_handle, truncated_key.data(), nullptr, &required_size));
    if(err != ESP_OK)
        goto out;

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
        goto out;

    rc = 0;
out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::load(const char *key, uint32_t &data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_u32(nvs_handle, truncated_key.data(), &data));
    if(err != ESP_OK)
        goto out;

    rc = 0;
out:
    nvs_close(nvs_handle);
    return rc;
}

int EspStorage::load(const char *key, float &data)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    int rc = -1;

    etl::string<15> truncated_key{key};

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle));
    if(err != ESP_OK)
        goto out;

    uint32_t value;
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        err = nvs_get_u32(nvs_handle, truncated_key.data(), &value));
    if(err != ESP_OK)
        goto out;

    memcpy(&data, &value, sizeof value);

    rc = 0;
out:
    nvs_close(nvs_handle);
    return rc;
}

} // namespace shrapnel::persistence