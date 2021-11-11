#include "PluginProcessor.h"
#include <cmath>
#include <assert.h>
#include "abstract_dsp.h"

#define MAX_DELAY_MS 15.f

/* The low pass filter on the control signal has very low cut-off frequency. It
 * needs to be run at a lower samplerate to ensure the filter's stability.
 */
#define CONTROL_SIGNAL_DOWNSAMPLE_ORDER (4)
#define CONTROL_SIGNAL_DOWNSAMPLE_RATIO (1 << CONTROL_SIGNAL_DOWNSAMPLE_ORDER)

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
            std::make_unique<juce::AudioParameterFloat> ("modulationRateHz",
                                                         "Rate (Hz)",
                                                         0.1f,
                                                         2.f,
                                                         0.5f),

            std::make_unique<juce::AudioParameterFloat> ("modulationDepth",
                                                         "Depth",
                                                         0.f,
                                                         1.f,
                                                         1.f),

            std::make_unique<juce::AudioParameterFloat> ("mix",
                                                         "Mix",
                                                         0.f,
                                                         1.f,
                                                         1.f),
        }),
     control_signal_downsample(1,
                               CONTROL_SIGNAL_DOWNSAMPLE_ORDER,
                               juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple)
{
    modulationRateHzParameter = parameters.getRawParameterValue("modulationRateHz");
    modulationDepthNormalisedParameter = parameters.getRawParameterValue("modulationDepth");
    mixParameter = parameters.getRawParameterValue("mix");

    lowpass = dspal_iir_create(2);
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // TODO this leaks the delayline when prepare called multiple times (reaper
    // calls this a couple times when we start playing)
    delayline = dspal_delayline_create(sampleRate * MAX_DELAY_MS / 1000);
    dspal_delayline_set_buffer_size(delayline, (size_t)samplesPerBlock);

    this->sampleRate = sampleRate;

    // TODO generate the low frequency noise at some low samplerate (~ 100 Hz)
    // IIR filters will get unstable if the cutoff frequency is very small.
    //
    // This can be done by keeping a sample counter, and when the count exceeds
    // some limit (the ratio of normal rate to low rate), processing a single
    // sample in the slow rate domain.
    //
    // E.G. every 256/512/1024 sample at 48kHz/96kHz/192kHz, process a sample.
    // This means the slow sample rate is 187.5 Hz.
    //
    // dsp::Oversampling can be used to oversample the slow rate to find the
    // signal value for every high samplerate sample.

    dspal_iir_reset(lowpass);

    control_signal_downsample.initProcessing((size_t)samplesPerBlock);
    control_signal_downsample.reset();
    noise_samples_left = 0;
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

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        if(channel == 0)
        {
            float coeffs[5] = { 0 };
            float normalised_cutoff = *modulationRateHzParameter /
                                       sampleRate *
                                       CONTROL_SIGNAL_DOWNSAMPLE_RATIO;
            dspal_biquad_design_lowpass(coeffs, normalised_cutoff, M_SQRT1_2);

            float noise = 0;
            float *noise_p = &noise;
            auto slow_noise_block = juce::dsp::AudioBlock<float>(&noise_p, 1, 1);
            juce::dsp::AudioBlock<float> fast_noise_block;
            dspal_iir_set_coeffs(lowpass, coeffs, 2);

            for(int i = 0; i < buffer.getNumSamples(); i++)
            {
                if(noise_samples_left == 0)
                {
                    noise = (random.nextFloat() - 0.5f) * 2;

                    dspal_iir_process(lowpass, &noise, &noise, 1);
                    /* Make up level loss from low pass filter */
                    noise /= normalised_cutoff;
                    noise *= juce::Decibels::decibelsToGain(-50.f);

                    fast_noise_block = control_signal_downsample.processSamplesUp(slow_noise_block);
                    assert(fast_noise_block.getNumSamples() == CONTROL_SIGNAL_DOWNSAMPLE_RATIO);
                    noise_samples_left = fast_noise_block.getNumSamples();
                }

                float current_noise_sample = fast_noise_block.getSample(
                        0,
                        (int)fast_noise_block.getNumSamples() -
                        noise_samples_left);
                noise_samples_left--;

#if 1
                float delay = MAX_DELAY_MS / 1000 * sampleRate *
                    (0.5f + (*modulationDepthNormalisedParameter * current_noise_sample));

                float clipped_delay = juce::jlimit((float)0, std::floor(sampleRate * MAX_DELAY_MS / 1000), delay);

                dspal_delayline_set_delay(delayline, clipped_delay);

                dspal_delayline_push_sample(delayline, channelData[i]);
                channelData[i] = channelData[i] +
                    (*mixParameter * dspal_delayline_pop_sample(delayline));
#else
                channelData[i] = current_noise_sample;
#endif
            }
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
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
