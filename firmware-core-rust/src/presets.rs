use fstr::FStr;

use crate::selected_preset::PresetId;

struct PresetName(FStr<32>);

pub(crate) struct ParametersData {
    amp_gain: f32,
    amp_channel: f32,
    bass: f32,
    middle: f32,
    treble: f32,
    contour: f32,
    volume: f32,
    noise_gate_threshold: f32,
    noise_gate_hysteresis: f32,
    noise_gate_attack: f32,
    noise_gate_hold: f32,
    noise_gate_release: f32,
    noise_gate_bypass: f32,
    chorus_rate: f32,
    chorus_depth: f32,
    chorus_mix: f32,
    chorus_bypass: f32,
    wah_position: f32,
    wah_vocal: f32,
    wah_bypass: f32,
}

pub(crate) struct PresetData {
    name: PresetName,
    parameter: ParametersData,
}

pub(crate) enum ApiMessage {
    Initialise,
    Notify { id: PresetId, preset: PresetData },
    Create { preset: PresetData },
    Update { id: PresetId, preset: PresetData },
    Delete { id: PresetId },
}
