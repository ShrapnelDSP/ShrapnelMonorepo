use num_enum::TryFromPrimitive;

#[derive(Debug, PartialEq, Eq)]
pub struct MidiMessage {
    pub channel: u8,
    pub parameters: MidiMessageParameters,
}

#[derive(Debug, PartialEq, Eq)]
pub enum MidiMessageParameters {
    NoteOn {
        note: u8,
        velocity: u8,
    },
    NoteOff {
        note: u8,
        velocity: u8,
    },
    ControlChange {
        control: u8,
        value: u8,
    },
    ProgramChange {
        program: u8,
    },
}

#[derive(TryFromPrimitive)]
#[repr(u8)]
pub(super) enum MessageType {
    NoteOff = 0x80,
    NoteOn = 0x90,
    ControlChange = 0xB0,
    ProgramChange = 0xC0,
}
