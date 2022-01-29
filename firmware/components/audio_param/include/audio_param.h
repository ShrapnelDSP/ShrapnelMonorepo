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
    public:
    AudioParametersBase() {};

    /** Update a parameter
     *
     * \param param Name of the parameter to update
     * \param value New normalised value of the parameter
     *
     * \return 0 on success
     */
    virtual int update(const std::string param, float value) = 0;

    virtual int createAndAddParameter(
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

    int createAndAddParameter(
        std::string name,
        float minimum,
        float maximum,
        float default_value) override;


    private:
    // TODO how big does this need to be?
    etl::map<std::string, std::unique_ptr<AudioParameterFloat>, 10> parameters;
};

};
