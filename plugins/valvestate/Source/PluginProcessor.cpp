/*
    Copyright 2020 Barabas Raffai

    This file is part of Valvestate Sim.

    Valvestate Sim is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    Valvestate Sim is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License along
    with Valvestate Sim.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

ValvestateAudioProcessor::ValvestateAudioProcessor() : 
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    logRange(0, 1, 0.0001, 0.3),
    parameters(*this, nullptr, "PARAMETERS",
        {
        std::make_unique<juce::AudioParameterBool>  ("od", "OD1/OD2", false),
        std::make_unique<juce::AudioParameterFloat> ("gain", "Gain", logRange, logRange.convertFrom0to1(0.5)),
        std::make_unique<juce::AudioParameterFloat> ("bass", "Bass", logRange, logRange.convertFrom0to1(0.5)),
        std::make_unique<juce::AudioParameterFloat> ("middle" , "Middle", 0, 1, 0.5),
        std::make_unique<juce::AudioParameterFloat> ("treble", "Treble", 0, 1, 0.5),
        //contour gets unstable when set to 0
        std::make_unique<juce::AudioParameterFloat> ("contour", "Contour", 0.01, 1, 0.5),
        std::make_unique<juce::AudioParameterFloat> ("volume", "Volume", 20, 50, 35)
        })
{
    od = parameters.getRawParameterValue("od");
    gain = parameters.getRawParameterValue("gain");
    bass = parameters.getRawParameterValue("bass");
    middle = parameters.getRawParameterValue("middle");
    treble = parameters.getRawParameterValue("treble");
    contourP = parameters.getRawParameterValue("contour");
    volume = parameters.getRawParameterValue("volume");
}

ValvestateAudioProcessor::~ValvestateAudioProcessor()
{
}

//==============================================================================
const juce::String ValvestateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ValvestateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ValvestateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ValvestateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ValvestateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ValvestateAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ValvestateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ValvestateAudioProcessor::setCurrentProgram (int index)
{
    (void) index;
}

const juce::String ValvestateAudioProcessor::getProgramName (int index)
{
    (void) index;
    return {};
}

void ValvestateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void) index;
    (void) newName;
}

//==============================================================================
void ValvestateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    jassert(samplesPerBlock >= 0);
    (void)samplesPerBlock;

    input.prepare(sampleRate);
    gaincontrol.prepare(sampleRate);
    fmv.prepare(sampleRate);
    contour.prepare(sampleRate);
}

void ValvestateAudioProcessor::releaseResources()
{
    input.reset();
    gaincontrol.reset();
    fmv.reset();
    contour.reset();
}

bool ValvestateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void ValvestateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    (void) midiMessages;

    juce::ScopedNoDenormals noDenormals;

    float *input_samples = buffer.getWritePointer(0);

    juce::dsp::AudioBlock<float> block(&input_samples, 1, (size_t)buffer.getNumSamples());
    juce::dsp::ProcessContextReplacing<float> context(block);

    //set parameters
    gaincontrol.set_parameters(*gain, *od);
    fmv.set_parameters(*bass, *middle, *treble);
    contour.setParameter(*contourP);

    //process data
    input.process(input_samples, (std::size_t)buffer.getNumSamples());
    gaincontrol.process(input_samples, (std::size_t)buffer.getNumSamples());
    clipping.process(input_samples, (std::size_t)buffer.getNumSamples());
    fmv.process(input_samples, (std::size_t)buffer.getNumSamples());
    contour.process(input_samples, (std::size_t)buffer.getNumSamples());

    block.multiplyBy(juce::Decibels::decibelsToGain((float)*volume));

    // copy processed samples to the right channel
    for(int i = 0; i < buffer.getNumSamples(); i++)
    {
        buffer.setSample(1, i, input_samples[i]);
    }
}

//==============================================================================
bool ValvestateAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ValvestateAudioProcessor::createEditor()
{
    return new ValvestateAudioProcessorEditor (*this);
}

//==============================================================================
void ValvestateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ValvestateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new ValvestateAudioProcessor();
}
