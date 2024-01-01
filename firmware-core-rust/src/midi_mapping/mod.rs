mod manager;

use crate::parameters::ParameterId;
use crate::midi_protocol::MidiMessage;

pub(crate) use manager::ParameterSetter;

pub(crate) struct MappingId(u32);

pub(crate) enum MappingMode {
    Parameter,
    Toggle,
}

pub(crate) struct Mapping {
    midi_channel: u8,
    cc_number: u8,
    mode: MappingMode,
    parameter_name: ParameterId,
}

pub(crate) enum ApiMessage {
    GetRequest,
    CreateRequest {
        mapping: Mapping,
    },
    CreateResponse {
        id: MappingId,
        mapping: Mapping,
    },
    Update {
        id: MappingId,
        mapping: Mapping,
    },
    Remove {
        id: MappingId,
    },
    MessageReceived {
        message: MidiMessage,
    }
}