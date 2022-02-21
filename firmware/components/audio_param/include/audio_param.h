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
#include <atomic>
#include <string>
#include <memory>
#include <vector>

namespace shrapnel {

class AudioParameterFloat {
    public:
    AudioParameterFloat(std::string name, float minimum, float maximum, float default_value);

    /** Update a parameter
     *
     * \param value New normalised value of the parameter
     */
    void update(float value);

    /** Get denormalised value of parameter
     */
    std::atomic<float> *get_raw_parameter(void);

    std::string name;

    private:
    std::atomic<float> value;
    float minimum;
    float maximum;
};

class AudioParametersBase {
    /* TODO how to avoid having to increase size manually when more effects are
     * added?
     *
     * We don't want to use heap allocation when the parameters are registered.
     * Maybe some constexpr functions could be used to register the parameters?
     *
     * ETL provides template dedcution guide and make template for etl::map.
     * These deduce the size from the initialiser. Passing an already
     * initialised etl::map using one of these methods to the constructor
     * should work.
     */
    public:
    using MapType = etl::map<std::string, std::unique_ptr<AudioParameterFloat>, 100>;

    AudioParametersBase() {};

    /** Update a parameter
     *
     * \param param Name of the parameter to update
     * \param value New normalised value of the parameter
     *
     * \return 0 on success
     */
    virtual int update(const std::string param, float value) = 0;

    virtual MapType::iterator begin() = 0;
    virtual MapType::iterator end() = 0;

    virtual int create_and_add_parameter(
        std::string name,
        float minimum,
        float maximum,
        float default_value) = 0;

    /** Get denormalised value of parameter
     *
     * \param param Name of the parameter to get
     */
    virtual std::atomic<float> *get_raw_parameter(const std::string param) = 0;
};

class AudioParameters : public AudioParametersBase {
    public:
    AudioParameters();

    int update(const std::string param, float value) override;
    std::atomic<float> *get_raw_parameter(const std::string param) override;

    int create_and_add_parameter(
        std::string name,
        float minimum,
        float maximum,
        float default_value) override;

    MapType::iterator begin() override;
    MapType::iterator end() override;

    private:
    MapType parameters;
};

};
