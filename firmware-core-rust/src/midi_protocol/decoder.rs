use crate::midi_protocol::MidiMessageParameters;
use crate::midi_protocol::*;
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

        match MessageType::try_from(self.current_status & 0xF0) {
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
        match MessageType::try_from(self.current_status & 0xF0) {
            Ok(MessageType::NoteOn) => Some(MidiMessage {
                channel,
                parameters: MidiMessageParameters::NoteOn(NoteOn {
                    note: self.received_data.0,
                    velocity: self.received_data.1,
                }),
            }),
            Ok(MessageType::NoteOff) => Some(MidiMessage {
                channel,
                parameters: MidiMessageParameters::NoteOff(NoteOff {
                    note: self.received_data.0,
                    velocity: self.received_data.1,
                }),
            }),
            Ok(MessageType::ControlChange) => Some({
                // Channel Mode messages should be ignored
                if (self.received_data.0 & 0xF8) == 0x78
                {
                    return None;
                }

                MidiMessage {
                    channel,
                    parameters: MidiMessageParameters::ControlChange(ControlChange {
                        control: self.received_data.0,
                        value: self.received_data.1,
                    }),
                }
            }),
            Ok(MessageType::ProgramChange) => Some(MidiMessage {
                channel,
                parameters: MidiMessageParameters::ProgramChange(ProgramChange {
                    program: self.received_data.0,
                }),
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
    use mockall::{automock, predicate, Sequence};
    use super::*;

    #[automock]
    trait Receiver {
        fn notify(&mut self, message: &MidiMessage);
    }

    #[test]
    fn note_on() {
        let expected = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::NoteOn(NoteOn {
                note: 0,
                velocity: 1,
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0x90, 0x00, 0x01] {
            sut.decode(byte);
        }
    }

    #[test]
    fn note_off() {
        let expected = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::NoteOff(NoteOff {
                note: 0,
                velocity: 1,
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0x80, 0x00, 0x01] {
            sut.decode(byte);
        }
    }

    #[test]
    fn program_change() {
        let expected = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::ProgramChange(ProgramChange {
                program: 0
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0xC0, 0x00] {
            sut.decode(byte);
        }
    }

    #[test]
    fn note_on_running_status() {
        let expected_first = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::NoteOn(NoteOn {
                note: 0,
                velocity: 1,
            }),
        };

        let expected_second = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::NoteOn(NoteOn {
                note: 2,
                velocity: 3,
            }),
        };

        let mut mock = MockReceiver::new();

        let mut seq = Sequence::new();
        mock.expect_notify().with(predicate::eq(expected_first)).once().in_sequence(&mut seq).returning(|_| ());
        mock.expect_notify().with(predicate::eq(expected_second)).once().in_sequence(&mut seq).returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0x90, 0x00, 0x01, 0x02, 0x03] {
            sut.decode(byte);
        }
    }

    #[test]
    fn control_change() {
        let expected = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::ControlChange(ControlChange {
                control: 0,
                value: 1,
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0xB0, 0x00, 0x01] {
            sut.decode(byte);
        }
    }

    #[test]
    fn channel_mode_is_not_control_change() {
        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::always()).never();

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for second_byte in 0x78..=0x7F {
            for byte in [0xB0, second_byte, 0x01] {
                sut.decode(byte);
            }
        }
    }

    #[test]
    fn channel_number() {
        let expected = MidiMessage {
            channel: 16,
            parameters: MidiMessageParameters::NoteOn(NoteOn {
                note: 0,
                velocity: 1,
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0x9F, 0x00, 0x01] {
            sut.decode(byte);
        }
    }

    #[test]
    fn ignores_unimplemented_messages() {
        let expected = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::NoteOn(NoteOn {
                note: 0,
                velocity: 1,
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0xE0, 0x00, 0x90, 0x00, 0x01] {
            sut.decode(byte);
        }
    }

    #[ignore]
    #[test]
    fn unimplemented_message_does_not_upset_running_status() {
        let expected = MidiMessage {
            channel: 1,
            parameters: MidiMessageParameters::NoteOn(NoteOn {
                note: 0,
                velocity: 1,
            }),
        };

        let mut mock = MockReceiver::new();
        mock.expect_notify().with(predicate::eq(expected)).once().returning(|_| ());

        let mut notify = |message: &_| mock.notify(message);
        let mut sut = MidiDecoder::new(&mut notify);
        for byte in [0x90, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x01] {
            sut.decode(byte);
        }
    }

    // TODO System exclusive message does not upset decoder
    // TODO Bank select:
    //      Control Change 0x00 followed by Control change 0x20, finally followed
    //      by program change is used to select a program from a 14-bit bank
    //      address.
    // TODO Real-Time messages may be sent at any time — even between bytes of a
    //      message which has a different status.
}
