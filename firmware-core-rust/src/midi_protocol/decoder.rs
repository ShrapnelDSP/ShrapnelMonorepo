use crate::midi_protocol::models::MidiMessageParameters::{ControlChange, NoteOff, NoteOn, ProgramChange};
use super::models::{MidiMessage, MessageType};

// TODO what's the difference between `dyn Fn` and `fn`
// generic trait for MidiDecoder might be more appropriate. It avoids the
// indirection of fn, and allows better optimisation.
// https://stackoverflow.com/a/27832320/15158846
pub type Callback<'a> = dyn FnMut(&MidiMessage) -> () + 'a;

pub struct MidiDecoder<'a> {
    on_message_received: &'a mut Callback<'a>,
    state: State,
    current_status: u8,
    data_count: usize,
    received_data: (u8, u8),
}

enum State {
    Idle,
    GotMessage,
}

impl MidiDecoder<'_> {
    /// Callback will be called from [`decode`] when the byte being decoded was
    /// the final byte of a message.
    pub fn new<'a>(callback: &'a mut Callback) -> MidiDecoder<'a> {
        return MidiDecoder {
            on_message_received: callback,
            state: State::Idle,
            current_status: 0,
            data_count: 0,
            received_data: (0, 0),
        };
    }

    /// Decode bytes received by UART.
    ///
    /// Call this function every time a byte has been received by UART.
    pub fn decode(&mut self, byte: u8) {
        self.state = match self.state {
            State::Idle => self.decode_idle(byte),
            State::GotMessage => self.decode_message(byte),
        };
    }

    fn decode_idle(&mut self, byte: u8) -> State {
        if !Self::is_status_byte(byte) && Self::is_status_byte(self.current_status)
        {
            return self.decode_message(byte);
        }

        match MessageType::try_from(byte & 0xF0) {
            Ok(_) => {
                self.current_status = byte;
                State::GotMessage
            }
            _ => State::Idle,
        }
    }

    fn decode_message(&mut self, byte: u8) -> State {
        assert!(Self::is_status_byte(self.current_status));

        match MessageType::try_from(self.current_status) {
            Ok(MessageType::ControlChange) | Ok(MessageType::NoteOn) | Ok(MessageType::NoteOff) => {
                match self.data_count {
                    0 => self.received_data.0 = byte,
                    1 => self.received_data.1 = byte,
                    _ => assert!(false)
                };

                self.data_count += 1;
                if self.data_count == 2
                {
                    self.output_message();
                    self.data_count = 0;
                    return State::Idle;
                }
            }
            Ok(MessageType::ProgramChange) => {
                self.received_data.0 = byte;
                self.output_message();
                return State::Idle;
            }
            _ => return State::GotMessage,
        }

        return State::GotMessage;
    }

    fn output_message(&mut self) {
        let message: Option<MidiMessage> = self.get_message();

        match message {
            Some(message) => (self.on_message_received)(&message),
            _ => {}
        };
    }

    fn get_message(&self) -> Option<MidiMessage> {
        let channel = (self.current_status & 0x0F) + 1;
        match MessageType::try_from(self.current_status) {
            Ok(MessageType::NoteOn) => Some(MidiMessage {
                channel,
                parameters: NoteOn {
                    note: self.received_data.0,
                    velocity: self.received_data.1,
                },
            }),
            Ok(MessageType::NoteOff) => Some(MidiMessage {
                channel,
                parameters: NoteOff {
                    note: self.received_data.0,
                    velocity: self.received_data.1,
                },
            }),
            Ok(MessageType::ControlChange) => Some({
                // Channel Mode messages should be ignored
                if (self.received_data.0 & 0xF8) == 0x78
                {
                    return None;
                }

                MidiMessage {
                    channel,
                    parameters: ControlChange {
                        control: self.received_data.0,
                        value: self.received_data.1,
                    },
                }
            }),
            Ok(MessageType::ProgramChange) => Some(MidiMessage {
                channel,
                parameters: ProgramChange {
                    program: self.received_data.0,
                },
            }),
            _ => None,
        }
    }

    fn is_status_byte(byte: u8) -> bool {
        (byte & (1 << 7)) != 0
    }
}

#[cfg(test)]
mod tests {
    use mockall::{automock, predicate};
    use crate::midi_protocol::models::MidiMessageParameters::NoteOn;
    use super::*;

    #[automock]
    trait Receiver {
        fn notify(&mut self, message: &MidiMessage);
    }

    #[test]
    fn note_on() {
        let expected = MidiMessage {
            channel: 1,
            parameters: NoteOn {
                note: 0,
                velocity: 1,
            },
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0x90, 0x00, 0x01] {
            sut.decode(byte);
        }
    }
}
