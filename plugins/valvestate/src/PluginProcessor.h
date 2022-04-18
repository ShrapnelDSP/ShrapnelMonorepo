
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "valvestate.h"

class ValvestateAudioProcessor  : public juce::AudioProcessor
{
public:
    ValvestateAudioProcessor();
    ~ValvestateAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    using AudioProcessor::processBlock;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::NormalisableRange<float> logRange;

public:
    juce::AudioProcessorValueTreeState parameters;

private:
    std::atomic<float> *od = nullptr;
    std::atomic<float> *gain = nullptr;
    std::atomic<float> *bass = nullptr;
    std::atomic<float> *middle = nullptr;
    std::atomic<float> *treble = nullptr;
    std::atomic<float> *contourP = nullptr;
    std::atomic<float> *volume = nullptr;

    shrapnel::effect::valvestate::Valvestate valvestate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValvestateAudioProcessor)
};
