mod manager;

use crate::midi_protocol::MidiMessage;
use crate::parameters::ParameterId;

pub(crate) use manager::ParameterSetter;

#[derive(Debug, PartialEq, Eq)]
pub(crate) struct MappingId(u32);

#[derive(Debug, PartialEq, Eq, Clone)]
pub(crate) enum MappingMode {
    Parameter,
    Toggle,
}

#[derive(Debug, PartialEq, Eq, Clone)]
pub(crate) struct Mapping {
    midi_channel: u8,
    cc_number: u8,
    mode: MappingMode,
    parameter_name: ParameterId,
}

#[derive(Debug, PartialEq, Eq)]
pub(crate) enum ApiMessage {
    GetRequest,
    CreateRequest { mapping: Mapping },
    CreateResponse { id: MappingId, mapping: Mapping },
    Update { id: MappingId, mapping: Mapping },
    Remove { id: MappingId },
    MessageReceived { message: MidiMessage },
}
