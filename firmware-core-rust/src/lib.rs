#![no_std]

extern crate alloc;

mod messages;
mod midi_mapping;
mod midi_protocol;
mod parameters;
mod audio_events;
mod selected_preset;
mod presets;
mod persistence;

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
