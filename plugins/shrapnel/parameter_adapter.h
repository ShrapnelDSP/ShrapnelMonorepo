#include "main_thread.h"
#include <juce_audio_processors/juce_audio_processors.h>

/** Adapts a JUCE AudioProcessorValueTreeState to work with shrapnel main thread
 * 
 */

namespace shrapnel {
class ParameterAdapter : private juce::AudioProcessorValueTreeState::Listener
{
    using MapType = etl::map<parameters::id_t,
                             std::unique_ptr<parameters::AudioParameterFloat>,
                             20>;
    static constexpr char TAG[] = "ParameterAdapter";

public:
    explicit ParameterAdapter(juce::AudioProcessorValueTreeState &a_parameters)
        : parameters{a_parameters}
    {
        // XXX: These are duplicated in the JUCE plugin, be sure to update both at
        // the same time
        create_and_add_parameter("ampGain", 0, 1, 0.5);
        create_and_add_parameter("ampChannel", 0, 1, 0);
        create_and_add_parameter("bass", 0, 1, 0.5);
        create_and_add_parameter("middle", 0, 1, 0.5);
        create_and_add_parameter("treble", 0, 1, 0.5);
        //contour gets unstable when set to 0
        create_and_add_parameter("contour", 0.01, 1, 0.5);
        create_and_add_parameter("volume", -30, 0, -15);

        create_and_add_parameter("noiseGateThreshold", -80, 0, -60);
        create_and_add_parameter("noiseGateHysteresis", 0, 5, 0);
        create_and_add_parameter("noiseGateAttack", 1, 50, 10);
        create_and_add_parameter("noiseGateHold", 1, 250, 50);
        create_and_add_parameter("noiseGateRelease", 1, 250, 50);
        create_and_add_parameter("noiseGateBypass", 0, 1, 0);

        create_and_add_parameter("chorusRate", 0.1, 4, 0.95);
        create_and_add_parameter("chorusDepth", 0, 1, 0.3);
        create_and_add_parameter("chorusMix", 0, 1, 0.8);
        create_and_add_parameter("chorusBypass", 0, 1, 1);

        create_and_add_parameter("wahPosition", 0, 1, 0.5);
        create_and_add_parameter("wahVocal", 0, 1, 0);
        create_and_add_parameter("wahBypass", 0, 1, 1);

        parameters.addParameterListener("ampGain", this);
        parameters.addParameterListener("ampChannel", this);
        parameters.addParameterListener("bass", this);
        parameters.addParameterListener("middle", this);
        parameters.addParameterListener("treble", this);
        parameters.addParameterListener("contour", this);
        parameters.addParameterListener("volume", this);
        parameters.addParameterListener("noiseGateThreshold", this);
        parameters.addParameterListener("noiseGateHysteresis", this);
        parameters.addParameterListener("noiseGateAttack", this);
        parameters.addParameterListener("noiseGateHold", this);
        parameters.addParameterListener("noiseGateRelease", this);
        parameters.addParameterListener("noiseGateBypass", this);
        parameters.addParameterListener("chorusRate", this);
        parameters.addParameterListener("chorusDepth", this);
        parameters.addParameterListener("chorusMix", this);
        parameters.addParameterListener("chorusBypass", this);
        parameters.addParameterListener("wahPosition", this);
        parameters.addParameterListener("wahVocal", this);
        parameters.addParameterListener("wahBypass", this);
    }

    int update(const parameters::id_t &id, float value)
    {
        // TODO:
        // Note that to make sure the host correctly handles automation, you should call
        // the beginChangeGesture() and endChangeGesture() methods to tell the host when
        // the user has started and stopped changing the parameter.
        //
        // This is not implemented in the GUI at the moment
        auto parameter = parameters.getParameter(id.data());
        if(parameter == nullptr)
        {
            return -1;
        }

        // It should be safe to call this from a background thread:
        // https://forum.juce.com/t/calling-setvaluenotifyinghost-from-processblock/26073/9

        // TODO this may expect a value between 0 and 1 like getValue returns
        parameter->setValueNotifyingHost(parameter->convertFrom0to1(value));
        return 0;
    }

    float get(const parameters::id_t &id)
    {
        auto parameter = parameters.getParameter(id.data());
        if(parameter == nullptr)
        {
            juce::Logger::outputDebugString(
                "Failed to get parameter with id: " + juce::String(id.data()));
            return 0.5;
        }

        return parameter->getValue();
    }

    // TODO this is not correct, need to make sure the iterator doesn't get
    // invalidated between calls to begin and end.
    //
    // If iteration and updates all happened in the same thread, then it would
    // be OK. We can move the updates to the main thread by sending updated
    // events to a queue in the listener, and handling them in the main thread.
    MapType::iterator begin() { return parameterValues.begin(); }

    MapType::iterator end() { return parameterValues.end(); }

private:
    void create_and_add_parameter(const parameters::id_t &name,
                                  float minimum,
                                  float maximum,
                                  float default_value)
    {
        if(parameterValues.full())
        {
            jassertfalse;
        }

        parameterValues[name] =
            std::make_unique<parameters::AudioParameterFloat>(
                name, minimum, maximum, default_value);

        ESP_LOGI(TAG, "%zu parameters are registered", parameterValues.size());
    }

    void parameterChanged(const juce::String &id, float value) override
    {
        auto id_str = id.toStdString();
        ESP_LOGD(TAG, "parameter update %s to %f", id_str.c_str(), value);
        parameterValues[parameters::id_t{id_str.c_str()}]->update(value);
    }

    juce::AudioProcessorValueTreeState &parameters;
    MapType parameterValues;
};

static_assert(UpdatableParameter<ParameterAdapter> &&
              GettableParameter<ParameterAdapter>);

} // namespace shrapnel
