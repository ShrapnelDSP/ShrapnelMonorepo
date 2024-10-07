#include "main_thread.h"
#include <juce_audio_processors/juce_audio_processors.h>

/** Adapts a JUCE AudioProcessorValueTreeState to work with shrapnel main thread
 * 
 */

namespace shrapnel {

template <typename AudioParametersT>
    requires UpdatableParameter<AudioParametersT> &&
             GettableParameter<AudioParametersT>
class ParameterAdapter : private juce::AudioProcessorValueTreeState::Listener
{
    static constexpr char TAG[] = "ParameterAdapter";

public:
    explicit ParameterAdapter(juce::AudioProcessorValueTreeState &a_parameters)
        : apvts{a_parameters}
    {
        apvts.addParameterListener("ampGain", this);
        apvts.addParameterListener("ampChannel", this);
        apvts.addParameterListener("bass", this);
        apvts.addParameterListener("middle", this);
        apvts.addParameterListener("treble", this);
        apvts.addParameterListener("contour", this);
        apvts.addParameterListener("volume", this);
        apvts.addParameterListener("noiseGateThreshold", this);
        apvts.addParameterListener("noiseGateHysteresis", this);
        apvts.addParameterListener("noiseGateAttack", this);
        apvts.addParameterListener("noiseGateHold", this);
        apvts.addParameterListener("noiseGateRelease", this);
        apvts.addParameterListener("noiseGateBypass", this);
        apvts.addParameterListener("chorusRate", this);
        apvts.addParameterListener("chorusDepth", this);
        apvts.addParameterListener("chorusMix", this);
        apvts.addParameterListener("chorusBypass", this);
        apvts.addParameterListener("wahPosition", this);
        apvts.addParameterListener("wahVocal", this);
        apvts.addParameterListener("wahBypass", this);
    }

    int update(const parameters::id_t &id, float value)
    {
        ESP_LOGD(TAG, "update %s %f", id.c_str(), value);

        // TODO:
        // Note that to make sure the host correctly handles automation, you should call
        // the beginChangeGesture() and endChangeGesture() methods to tell the host when
        // the user has started and stopped changing the parameter.
        //
        // This is not implemented in the GUI at the moment
        auto parameter = apvts.getParameter(id.data());
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

private:
    void parameterChanged(const juce::String &id, float value) override
    {
        // FIXME: what thread is calling this? Probably need to send a message
        // and handle on the main thread.
        auto id_str = id.toStdString();
        ESP_LOGD(TAG, "parameterChanged %s %f", id_str.c_str(), value);

        on_parameter_updated_by_host(id_str.c_str(), value);
    }

    juce::AudioProcessorValueTreeState &apvts;
    etl::delegate<void(const char *id, float value)>
        on_parameter_updated_by_host;
};

} // namespace shrapnel
