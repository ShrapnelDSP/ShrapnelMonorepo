use crate::{audio_events, midi_mapping, parameters, presets, selected_preset};

pub(crate) enum ApiMessage {
    Parameters(parameters::ApiMessage),
    Midi(midi_mapping::ApiMessage),
    Events(audio_events::ApiMessage),
    SelectedPreset(selected_preset::ApiMessage),
    Presets(presets::ApiMessage),
}
