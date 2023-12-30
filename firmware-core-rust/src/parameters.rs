use fstr::FStr;

pub(crate) struct ParameterId(FStr<32>);

pub(crate) enum ApiMessage {
    Update {
        id: ParameterId,
        value: f32,
    },
    Initialise,
}