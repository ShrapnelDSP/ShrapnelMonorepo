use num_enum::TryFromPrimitive;

pub const CC_VALUE_MAX: u8 = 127;

#[derive(Debug, PartialEq, Eq)]
pub struct MidiMessage {
    pub channel: u8,
    pub parameters: MidiMessageParameters,
}

#[derive(Debug, PartialEq, Eq)]
pub struct NoteOn {
    pub note: u8,
    pub velocity: u8,
}

#[derive(Debug, PartialEq, Eq)]
pub struct NoteOff {
    pub note: u8,
    pub velocity: u8,
}

#[derive(Debug, PartialEq, Eq)]
/// The value of a continuous controller has changed
pub struct ControlChange {
    /// The controller number
    pub control: u8,
    /// The value of the controller
    pub value: u8,
}

#[derive(Debug, PartialEq, Eq)]
pub struct ProgramChange {
    pub program: u8,
}

#[derive(Debug, PartialEq, Eq)]
pub enum MidiMessageParameters {
    NoteOn(NoteOn),
    NoteOff(NoteOff),
    ControlChange(ControlChange),
    ProgramChange(ProgramChange),
}

#[derive(TryFromPrimitive)]
#[repr(u8)]
pub(super) enum MessageType {
    NoteOff = 0x80,
    NoteOn = 0x90,
    ControlChange = 0xB0,
    ProgramChange = 0xC0,
}
