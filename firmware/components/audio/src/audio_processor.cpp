#include "audio_processor.h"
#include "esp_log.h"
#include "speaker_coeffs.h"
#include <noise_gate.h>

#define TAG "audio_processor"

namespace {

float decibel_to_ratio(float db) { return powf(10.f, db / 20.f); }

} // namespace

namespace shrapnel {

AudioProcessor::AudioProcessor(AudioProcessorParameters a_parameters)
    : parameters{a_parameters},
      speaker{{std::make_unique<shrapnel::dsp::FastConvolution<1024, 512>>(fir_coeff)}}
{
    esp_err_t rc = gate_init();
    assert(rc == ESP_OK);

    ESP_LOGI(TAG, "Initialised FIR filter with length %zd", fir_coeff.size());
}

void AudioProcessor::prepare(float sample_rate, size_t buffer_size)
{
    gate_set_sample_rate(sample_rate);
    gate_set_buffer_size(buffer_size);

    chorus.prepare(sample_rate, buffer_size);
    wah.prepare(sample_rate, buffer_size);
    valvestate.prepare(sample_rate, buffer_size);
}

void AudioProcessor::process(std::span<float, 512> fbuf)
{
    gate_set_threshold(*parameters.gate.threshold, *parameters.gate.hysteresis);
    gate_set_attack(*parameters.gate.attack);
    gate_set_hold(*parameters.gate.hold);
    gate_set_release(*parameters.gate.release);

    profiling_mark_stage(0);

    gate_analyse(fbuf.data(), fbuf.size());
    profiling_mark_stage(1);

    valvestate.set_gain(*parameters.amplifier.gain,
                        *parameters.amplifier.channel);
    valvestate.set_fmv(*parameters.amplifier.bass,
                       *parameters.amplifier.middle,
                       *parameters.amplifier.treble);
    valvestate.set_contour(*parameters.amplifier.contour);
    valvestate.set_volume(decibel_to_ratio(*parameters.amplifier.volume));
    profiling_mark_stage(2);

    valvestate.process(fbuf);
    profiling_mark_stage(3);

    if(*parameters.gate.bypass < 0.5f)
    {
        gate_process(fbuf.data(), fbuf.size());
    }
    profiling_mark_stage(4);

    /* speaker IR */
    speaker.process(fbuf);
    profiling_mark_stage(17);

    chorus.set_modulation_rate_hz(*parameters.chorus.rate);
    chorus.set_modulation_depth(*parameters.chorus.depth);
    chorus.set_modulation_mix(*parameters.chorus.mix);
    profiling_mark_stage(18);

    if(*parameters.chorus.bypass < 0.5f)
    {
        chorus.process(fbuf);
    }
    profiling_mark_stage(19);

    wah.set_expression_position(*parameters.wah.position);
    wah.set_vocal(*parameters.wah.vocal);

    if(*parameters.wah.bypass < 0.5f)
    {
        wah.process(fbuf);
    }
}

void AudioProcessor::reset() {
    valvestate.reset();
    chorus.reset();
    wah.reset();
    speaker.reset();
}

} // namespace shrapnel