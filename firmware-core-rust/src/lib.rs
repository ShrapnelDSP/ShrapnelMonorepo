#![no_std]

extern crate alloc;

mod api;
mod audio_events;
mod experiment_return_through_slice;
mod messages;
mod midi_mapping;
pub mod midi_protocol;
mod parameters;
mod persistence;
mod presets;
mod selected_preset;

#[cfg(test)]
extern crate std;
