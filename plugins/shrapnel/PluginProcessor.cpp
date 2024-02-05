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

#include "PluginProcessor.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      parameters{
          *this,
          nullptr,
          juce::Identifier("parameters"),
          {
              // XXX: These are duplicated in the firmware, be sure to
              // update both at the same time
              std::make_unique<juce::AudioParameterFloat>(
                  "ampGain", "Amplifier Gain", 0.f, 1.f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>(
                  "ampChannel", "Amplifier Channel", 0.f, 1.f, 0.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "bass", "Amplifier Bass", 0.f, 1.f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>(
                  "middle", "Amplifier Middle", 0.f, 1.f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>(
                  "treble", "Amplifier Treble", 0.f, 1.f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>(
                  "contour", "Amplifier Contour", 0.01f, 1.f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>(
                  "volume", "Amplifier Volume", -30.f, 0.0f, -15.f),

              std::make_unique<juce::AudioParameterFloat>(
                  "noiseGateThreshold", "Gate Threshold", -80.f, 0.0f, -60.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "noiseGateHysteresis", "Gate Hysteresis", 0.f, 5.0f, 0.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "noiseGateAttack", "Gate Attack", 1.f, 50.0f, 10.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "noiseGateHold", "Gate Hold", 1.f, 250.0f, 50.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "noiseGateRelease", "Gate Release", 1.f, 250.0f, 50.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "noiseGateBypass", "Gate Bypass", 0.f, 1.f, 0.f),

              std::make_unique<juce::AudioParameterFloat>(
                  "chorusRate", "Chorus Rate", 0.1f, 4.f, 0.95f),
              std::make_unique<juce::AudioParameterFloat>(
                  "chorusDepth", "Chorus Depth", 0.f, 1.f, 0.3f),
              std::make_unique<juce::AudioParameterFloat>(
                  "chorusMix", "Chorus Mix", 0.f, 1.f, 0.8f),
              std::make_unique<juce::AudioParameterFloat>(
                  "chorusBypass", "Chorus Bypass", 0.f, 1.0f, 1.f),

              std::make_unique<juce::AudioParameterFloat>(
                  "wahPosition", "Wah Position", 0.f, 1.f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>(
                  "wahVocal", "Wah Vocal", 0.f, 1.f, 0.f),
              std::make_unique<juce::AudioParameterFloat>(
                  "wahBypass", "Wah Bypass", 0.f, 1.f, 1.f),
          },
      },
      processor{
          // clang-format off
          ShrapnelAudioProcessor{
              {
                  .amplifier{
                      .gain{parameters.getRawParameterValue("ampGain")},
                      .channel{parameters.getRawParameterValue("ampChannel")},
                      .bass{parameters.getRawParameterValue("bass")},
                      .middle{parameters.getRawParameterValue("middle")},
                      .treble{parameters.getRawParameterValue("treble")},
                      .contour{parameters.getRawParameterValue("contour")},
                      .volume{parameters.getRawParameterValue("volume")},
                  },
                  .gate{
                      .threshold{parameters.getRawParameterValue( "noiseGateThreshold")},
                      .hysteresis{parameters.getRawParameterValue( "noiseGateHysteresis")},
                      .attack{ parameters.getRawParameterValue("noiseGateAttack")},
                      .hold{parameters.getRawParameterValue("noiseGateHold")},
                      .release{ parameters.getRawParameterValue("noiseGateRelease")},
                      .bypass{ parameters.getRawParameterValue("noiseGateBypass")},
                  },
                  .chorus{
                      .rate{parameters.getRawParameterValue("chorusRate")},
                      .depth{parameters.getRawParameterValue("chorusDepth")},
                      .mix{parameters.getRawParameterValue("chorusMix")},
                      .bypass{parameters.getRawParameterValue("chorusBypass")},
                  },
                  .wah{
                      .position{parameters.getRawParameterValue("wahPosition")},
                      .vocal{parameters.getRawParameterValue("wahVocal")},
                      .bypass{parameters.getRawParameterValue("wahBypass")},
                  },
              },
          },
          // clang-format on
      }
{
    bool success = main_thread.startThread();
    jassert(success);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() = default;

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

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms()
{
    // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
    return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock)
{
    processor.prepare(
        {sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2});
}

void AudioPluginAudioProcessor::releaseResources() { processor.reset(); }

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if(layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if(layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    // Sum input channels
    buffer.addFrom(0, 0, buffer.getReadPointer(1), buffer.getNumSamples());
    auto samples = buffer.getWritePointer(0);
    juce::dsp::AudioBlock<float> block{
        &samples, 1, static_cast<size_t>(buffer.getNumSamples())};

    processor.process(block);

    // copy processed output to other channel for mono output
    buffer.copyFrom(1, 0, samples, buffer.getNumSamples());
}

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
