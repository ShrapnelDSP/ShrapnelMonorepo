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

#pragma once

#include "nvs.h"
#include <esp_log.h>
#include <span>
#include <string_view>

namespace {
inline etl::string<15> id_to_key(id_t id)
{
    char hex[9];
    int rc = snprintf(hex, sizeof hex, "%08" PRIx32, id);
    assert(rc == 8);
    return {hex, 9};
}

inline id_t key_to_id(const etl::string<15> &key)
{
    id_t id;
    int rc = sscanf(key.c_str(), "%08" PRIx32, &id);
    assert(rc == 1);
    return id;
}
} // namespace

namespace shrapnel::presets_storage {

/** Interface for storing persistent data
*
* The data sent to \ref save should be reloaded by \ref load even after power
* down.
*/
template<size_t READ_BUFFER_SIZE>
class EspCrud : public persistence::Crud<std::span<uint8_t>>
{
public:
    [[nodiscard]] int create(const std::span<uint8_t> &data,
                             presets::id_t &id_out) override
    {
        nvs_handle_t nvs_handle;
        esp_err_t err;
        int rc = -1;

        // start at 0 in case last ID has not been saved to NVS
        uint32_t id = 0;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_open(namespace_name, NVS_READWRITE, &nvs_handle));
        if(err != ESP_OK)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_get_u32(nvs_handle, last_id_name, &id));
        if(err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
            goto out;

        id++;
        id_out = id;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_set_blob(
                nvs_handle, id_to_key(id).c_str(), data.data(), data.size()));
        if(err != ESP_OK)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_set_u32(nvs_handle, last_id_name, id));
        if(err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
        if(err != ESP_OK)
            goto out;

        rc = 0;

    out:
        nvs_close(nvs_handle);
        return rc;
    }

    /// \return  non-zero on error
    [[nodiscard]] int read(presets::id_t id,
                           std::span<uint8_t> &buffer) override
    {
        nvs_handle_t nvs_handle;
        esp_err_t err;
        std::size_t required_size = 0;
        int rc = -1;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_open(namespace_name, NVS_READONLY, &nvs_handle));
        if(err != ESP_OK)
            goto out;

        // query the required size
        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_get_blob(
                nvs_handle, id_to_key(id).c_str(), nullptr, &required_size));
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

        ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_get_blob(nvs_handle,
                                                         id_to_key(id).c_str(),
                                                         buffer.data(),
                                                         &required_size));
        if(err != ESP_OK)
            goto out;

        buffer = buffer.subspan(0, required_size);
        rc = 0;
    out:
        nvs_close(nvs_handle);
        return rc;
    };

    /// \return  non-zero on error
    [[nodiscard]] int update(presets::id_t id,
                             const std::span<uint8_t> &data) override
    {
        nvs_handle_t nvs_handle;
        esp_err_t err;
        int rc = -1;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_open(namespace_name, NVS_READWRITE, &nvs_handle));
        if(err != ESP_OK)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_set_blob(
                nvs_handle, id_to_key(id).c_str(), data.data(), data.size()));
        if(err != ESP_OK)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
        if(err != ESP_OK)
            goto out;

        rc = 0;

    out:
        nvs_close(nvs_handle);
        return rc;
    };

    /// \return  non-zero on error
    [[nodiscard]] int destroy(presets::id_t id) override
    {
        nvs_handle_t nvs_handle;
        esp_err_t err;
        int rc = -1;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_open(namespace_name, NVS_READWRITE, &nvs_handle));
        if(err != ESP_OK)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(
            err = nvs_erase_key(nvs_handle, id_to_key(id).c_str()));
        if(err != ESP_OK)
            goto out;

        ESP_ERROR_CHECK_WITHOUT_ABORT(err = nvs_commit(nvs_handle));
        if(err != ESP_OK)
            goto out;

        rc = 0;

    out:
        nvs_close(nvs_handle);
        return rc;
    };

    void for_each(etl::delegate<void(uint32_t, const std::span<uint8_t> &)>
                      callback) override
    {
        for(const auto &entry : *this)
        {
            std::array<uint8_t, READ_BUFFER_SIZE> memory;
            auto buffer = std::span<uint8_t>(memory);
            
            auto id = key_to_id(entry.key);
            auto rc = read(id, buffer);
            if(rc != 0)
            {
                ESP_LOGE(TAG, "Failed to read preset at key %s", entry.key);
                continue;
            }
            callback(id, buffer);
        }
    }

private:
    struct Iterator
    {
        using difference_type = int;
        using value_type = nvs_entry_info_t;

        // An end iterator
        Iterator()
            : part_name(nullptr),
              namespace_name(nullptr),
              type(NVS_TYPE_BLOB)
        {
        }

        Iterator(const char *part_name,
                 const char *namespace_name,
                 nvs_type_t type)
            : part_name(part_name),
              namespace_name(namespace_name),
              type(type)
        {
            esp_err_t rc =
                nvs_entry_find(part_name, namespace_name, type, &self);
            if(rc == ESP_ERR_NVS_NOT_FOUND)
            {
                assert(self == nullptr);
            }
            else if(rc != ESP_OK)
            {
                assert(self == nullptr);
                ESP_LOGE(TAG,
                         "nvs_entry_find failed: %d %s",
                         rc,
                         esp_err_to_name(rc));
            }
        }
        ~Iterator() { nvs_release_iterator(self); }

        Iterator(const Iterator &other)
            : part_name(other.part_name),
              namespace_name(other.namespace_name),
              type(other.type)
        {
            int rc = nvs_entry_find(part_name, namespace_name, type, &self);

            if(rc == ESP_ERR_NVS_NOT_FOUND)
            {
                assert(self == nullptr);
                return;
            }
            else if(rc != ESP_OK)
            {
                assert(self == nullptr);
                ESP_LOGE(TAG,
                         "nvs_entry_find failed: %d %s",
                         rc,
                         esp_err_to_name(rc));
                return;
            }

            for(i = 0; i < other.i; i++)
            {
                ESP_ERROR_CHECK(nvs_entry_next(&self));
            }
        }

        Iterator &operator=(const Iterator &other)
        {
            if(this == &other)
                return *this;

            i = 0;
            part_name = other.part_name;
            namespace_name = other.namespace_name;
            type = other.type;
            nvs_release_iterator(self);

            ESP_ERROR_CHECK(
                nvs_entry_find(part_name, namespace_name, type, &self));
            for(i = 0; i < other.i; i++)
            {
                ESP_ERROR_CHECK(nvs_entry_next(&self));
            }

            return *this;
        }

        nvs_entry_info_t operator*() const
        {
            nvs_entry_info_t info;
            ESP_ERROR_CHECK(nvs_entry_info(self, &info));
            return info;
        };

        Iterator &operator++()
        {
            next();
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            next();
            return tmp;
        }

        friend bool operator==(const Iterator &left, const Iterator &right)
        {
            // all end iterators are equal
            if(left.self == nullptr && right.self == nullptr)
                return true;

            // end iterator is not equal to any valid one
            if(left.self == nullptr || right.self == nullptr)
                return false;

            // pointers to the same iterator are equal
            if(left.self == right.self)
                return true;

            // if partitions are different, then the iterators can't be equal
            auto left_partition = std::basic_string_view(left.part_name);
            auto right_partition = std::basic_string_view(right.part_name);
            if(left_partition != right_partition)
                return false;

            // if namespaces are different, then the iterators can't be equal
            auto left_namespace =
                std::basic_string_view((*left).namespace_name);
            auto right_namespace =
                std::basic_string_view((*right).namespace_name);
            if(left_namespace != right_namespace)
                return false;

            auto left_key = std::basic_string_view((*left).key);
            auto right_key = std::basic_string_view((*right).key);
            if(left_key != right_key)
                return false;

            // type doesn't matter, these are pointing to the same thing
            return true;
        }

    private:
        void next()
        {
            i++;
            int rc = nvs_entry_next(&self);
            if(rc == ESP_ERR_NVS_NOT_FOUND)
            {
                assert(self == nullptr);
                return;
            }
            else if(rc != ESP_OK)
            {
                ESP_LOGE(TAG,
                         "nvs_entry_next failed: %d %s",
                         rc,
                         esp_err_to_name(rc));
            }
        }

        const char *part_name;
        const char *namespace_name;
        nvs_type_t type;
        int i = 0;
        nvs_iterator_t self = nullptr;
    };

    Iterator begin() { return {part_name, namespace_name, NVS_TYPE_BLOB}; }

    Iterator end() { return Iterator(); }

    char *part_name;
    char *namespace_name;

    static constexpr char last_id_name[] = "last_id";
    static constexpr char TAG[] = "presets_storage";

    static_assert(std::input_iterator<EspCrud::Iterator>);
};

} // namespace shrapnel::presets_storage