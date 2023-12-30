mod manager;

use crate::{midi_protocol, parameters};

pub(crate) struct MappingId(u32);

enum MappingMode {
    Parameter,
    Toggle,
}

pub(crate) struct Mapping {
    midi_channel: u8,
    cc_number: u8,
    mode: MappingMode,
    parameter_name: parameters::ParameterId,
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
        message: midi_protocol::MidiMessage,
    }
}