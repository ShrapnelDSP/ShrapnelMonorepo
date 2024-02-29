#include "main_thread.h"
#include <juce_audio_processors/juce_audio_processors.h>

/** Adapts a JUCE AudioProcessorValueTreeState to work with shrapnel main thread
 * 
 */
class ParameterAdapter
{
public:
    explicit ParameterAdapter(juce::AudioProcessorValueTreeState &a_parameters)
        : parameters{a_parameters}
    {
    }

    int update(const shrapnel::parameters::id_t &id, float value)
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
    }

    float get(const shrapnel::parameters::id_t &id)
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

    // TODO begin and end members to iterate all parameters
    // implemented using copyState, then iterating the returned tree
    MapType::iterator begin() { auto tree = parameters.copyState(); }

    MapType::iterator end() { return parameters.end(); }

private:
    juce::AudioProcessorValueTreeState &parameters;
};

static_assert(shrapnel::UpdatableParameter<ParameterAdapter> &&
              shrapnel::GettableParameter<ParameterAdapter>);