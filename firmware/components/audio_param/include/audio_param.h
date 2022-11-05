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
#include "etl/string.h"
#include <atomic>
#include <string>
#include <memory>
#include <vector>

namespace shrapnel {
namespace parameters {

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

class AudioParameters final {
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
    using MapType = etl::map<id_t, std::unique_ptr<AudioParameterFloat>, 100>;

    AudioParameters();

    /** Update a parameter
     *
     * \param param Name of the parameter to update
     * \param value New normalised value of the parameter
     *
     * \return 0 on success
     */
    int update(const id_t &param, float value);

    /** Get the normalised value of a parameter */
    float get(const id_t &param);

    /** Get denormalised value of parameter
     *
     * \param param Name of the parameter to get
     */
    std::atomic<float> *get_raw_parameter(const id_t &param);

    int create_and_add_parameter(
        const id_t &name,
        float minimum,
        float maximum,
        float default_value);

    MapType::iterator begin();
    MapType::iterator end();

    private:
    MapType parameters;
};

}
}
