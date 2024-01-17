#![no_std]

extern crate alloc;

mod api;
mod audio_events;
mod messages;
mod midi_mapping;
mod midi_protocol;
mod parameters;
mod persistence;
mod presets;
mod selected_preset;

pub fn add(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
extern crate std;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
