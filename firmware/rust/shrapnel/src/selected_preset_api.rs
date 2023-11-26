// FIXME: set up namespacing correctly
//mod shrapnel {
//mod selected_preset {

use serde::Deserialize;
use serde::Serialize;

/// Messages that can be received through the Websocket connection.
#[derive(Serialize, Deserialize, Debug, PartialEq)]
pub enum InputMessage<'a> {
    /// The client wants to get the current value of the selected preset
    Read {},
    /// The client wants to set the current value of the selected preset
    Write { id: &'a str },
}

/// Messages that can be sent through the Websocket connection.
pub enum OutputMessage<'a> {
    /// Notify the client in response to an InputMessage::Read
    Notify { id: &'a str },
}

#[cfg(test)]
mod tests {
    use serde_json_core::de::from_str;
    use crate::selected_preset_api::InputMessage;

    #[test]
    fn deseriaise_input_message() {
        let message = serde_json_core::ser::to_string::<InputMessage, 100>(&InputMessage::Read {});
        // FIXME: this is not the format expected by Dart, it wants a
        // "messageType": "Read" or similar name. The name of the field is
        // configurable per type. The rest of the fields are at the top level.
        assert_eq!(from_str::<InputMessage>(r#"{"Read" : {}}"#), Ok((InputMessage::Read {}, 13)));
    }
}