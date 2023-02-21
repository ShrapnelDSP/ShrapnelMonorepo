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

#include "etl/map.h"
#include "etl/observer.h"
#include "etl/string.h"
#include "esp_log.h"
#include <atomic>
#include <string>
#include <memory>
#include <vector>

namespace shrapnel {
namespace parameters {

constexpr const char *TAG = "audio_param";

using id_t = etl::string<32>;

class AudioParameterFloat {
    public:
    AudioParameterFloat(const id_t &name, float minimum, float maximum, float default_value);

    /** Update a parameter
     *
     * \param value New normalised value of the parameter
     */
    void update(float value);

    /** Get the normalised value of a parameter */
    float get(void);

    /** Get denormalised value of parameter
     */
    std::atomic<float> *get_raw_parameter(void);

    id_t name;

    private:
    std::atomic<float> value;
    float minimum;
    float maximum;
};

using ParameterObserver = etl::observer<std::pair<const id_t &, float>>;

class AudioParametersBase {
public:
    virtual std::atomic<float> *get_raw_parameter(const id_t &param) = 0;
};

template<const std::size_t MAX_PARAMETERS, const std::size_t MAX_OBSERVERS>
class AudioParameters final : public etl::observable<ParameterObserver, MAX_OBSERVERS>, public AudioParametersBase {
    public:
    using MapType = etl::map<id_t, std::unique_ptr<AudioParameterFloat>, MAX_PARAMETERS>;

    /** Update a parameter
     *
     * \param param Name of the parameter to update
     * \param value New normalised value of the parameter
     *
     * \return 0 on success
     */
    int update(const id_t &param, float value)
    {
        auto element = parameters.find(param);

        if(element == parameters.end())
        {
            return -1;
        }

        parameters[param]->update(value);
        this->notify_observers(std::move(std::pair{param, value}));

        ESP_LOGI(TAG, "Updated %.*s to %f", (int)param.length(), param.data(), value);

        return 0;
    }

    /** Get the normalised value of a parameter */
    float get(const id_t &param)
    {
        auto element = parameters.find(param);

        if(element == parameters.end())
        {
            return 0;
        }

        return parameters[param]->get();
    }

    /** Get denormalised value of parameter
     *
     * \param param Name of the parameter to get
     */
    std::atomic<float> *get_raw_parameter(const id_t &param) override
    {
        auto element = parameters.find(param);

        if(element == parameters.end())
        {
            return nullptr;
        }

        return parameters[param]->get_raw_parameter();
    }

    [[nodiscard]] int create_and_add_parameter(
        const id_t &name,
        float minimum,
        float maximum,
        float default_value)
    {
        if(parameters.full())
        {
            return -1;
        }

        parameters[name] = std::make_unique<AudioParameterFloat>(name, minimum, maximum, default_value);

        ESP_LOGI(TAG, "%zu parameters are registered", parameters.size());
        return 0;
    }

    MapType::iterator begin()
    {
        return parameters.begin();
    }

    MapType::iterator end()
    {
        return parameters.end();
    }

    private:
    MapType parameters;
};

}
}
