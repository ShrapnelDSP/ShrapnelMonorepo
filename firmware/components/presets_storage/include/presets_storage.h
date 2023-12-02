#include "nvs.h"
#include <esp_log.h>
#include <span>
#include <string_view>

namespace shrapnel::presets_storage {

/** Interface for storing persistent data
*
* The data sent to \ref save should be reloaded by \ref load even after power
* down.
*/
class Storage
{
public:
    Storage(const char *part_name, const char *namespace_name)
        : part_name(part_name),
          namespace_name(namespace_name)
    {
    }

    struct Iterator
    {
        using difference_type = int;
        using value_type = nvs_entry_info_t;

        // An end iterator
        Iterator()
            : part_name(nullptr),
              namespace_name(nullptr),
              type(NVS_TYPE_ANY)
        {
        }

        Iterator(const char *part_name,
                 const char *namespace_name,
                 nvs_type_t type)
            : part_name(part_name),
              namespace_name(namespace_name),
              type(type)
        {
            ESP_ERROR_CHECK(
                nvs_entry_find(part_name, namespace_name, type, &self));
        }
        ~Iterator() { nvs_release_iterator(self); }

        Iterator(const Iterator &other)
            : part_name(other.part_name),
              namespace_name(other.namespace_name),
              type(other.type)
        {
            ESP_ERROR_CHECK(
                nvs_entry_find(part_name, namespace_name, type, &self));
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
                         "nvs_entry_next falied: %d %s",
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

    /// \return  non-zero on error
    // virtual int save(const char *key, std::span<uint8_t> data) = 0;
    /// \return  non-zero on error
    // virtual int load(const char *key, std::span<uint8_t> &data) = 0;

    Iterator begin() { return {part_name, namespace_name, NVS_TYPE_ANY}; }

    Iterator end() { return Iterator(); }

private:
    const char *part_name;
    const char *namespace_name;
    static constexpr char TAG[] = "presets_storage";
};

static_assert(std::input_iterator<Storage::Iterator>);

} // namespace shrapnel::presets_storage