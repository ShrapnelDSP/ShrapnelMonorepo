#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "noise_gate.h"
#include "assert.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters(*this, nullptr, juce::Identifier("parameters"),
               {
                    std::make_unique<juce::AudioParameterFloat>("threshold",
                                                                "Threshold",
                                                                -80.f,
                                                                0.f,
                                                                -60.f),
                    std::make_unique<juce::AudioParameterFloat>("hysteresis",
                                                                "Hysteresis",
                                                                0.f,
                                                                5.f,
                                                                0.f),
                    std::make_unique<juce::AudioParameterFloat>("attack",
                                                                "Attack",
                                                                1.f,
                                                                50.f,
                                                                10.f),
                    std::make_unique<juce::AudioParameterFloat>("hold",
                                                                "Hold",
                                                                1.f,
                                                                250.f,
                                                                50.f),
                    std::make_unique<juce::AudioParameterFloat>("release",
                                                                "Release",
                                                                1.f,
                                                                250.f,
                                                                50.f),
               })
{
    thresholdParameter = parameters.getRawParameterValue("threshold");
    hysteresisParameter = parameters.getRawParameterValue("hysteresis");
    attackParameter = parameters.getRawParameterValue("attack");
    holdParameter = parameters.getRawParameterValue("hold");
    releaseParameter = parameters.getRawParameterValue("release");

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    assert(samplesPerBlock > 0);
    gate_set_buffer_size((size_t)samplesPerBlock);
    gate_set_sample_rate(sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    gate_set_threshold(*thresholdParameter);
    gate_set_hysteresis(*hysteresisParameter);
    gate_set_attack(*attackParameter);
    gate_set_hold(*holdParameter);
    gate_set_release(*releaseParameter);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if(channel == 0)
        {
            auto* channelData = buffer.getWritePointer (channel);
            gate_analyse(channelData, (size_t)buffer.getNumSamples());
            gate_process(channelData, (size_t)buffer.getNumSamples());
        }
        else
        {
            buffer.clear (channel, 0, buffer.getNumSamples());
        }
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    //return new AudioPluginAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
