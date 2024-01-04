use fstr::FStr;

#[derive(Debug, PartialEq, Eq)]
pub(crate) struct ParameterId(pub FStr<32>);

pub(crate) enum ApiMessage {
    Update { id: ParameterId, value: f32 },
    Initialise,
}
