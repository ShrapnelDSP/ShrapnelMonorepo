// FIXME: set up namespacing correctly
//mod shrapnel {
//mod selected_preset {

use serde::Deserialize;
// use serde::Serialize;

// Can't use serde(tag = "messageType") because it uses deserialize_any, which
// is not supported by serde_json_core deserializer.

// maybe need fake C++ style std::variant over enum to easily add the
// "messageType" field
struct Read {}
struct Write<'a> {
    id: &'a str,
}

/// Messages that can be received through the Websocket connection.
#[derive(Deserialize, Debug, PartialEq)]
#[serde(tag = "messageType")]
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
    use crate::selected_preset_api::InputMessage;
    use serde_json_core::de::from_str;

    #[test]
    fn deseriaise_input_read_message() {
        assert_eq!(
            from_str::<InputMessage>(r#"{"messageType" : "Read"}"#),
            Ok((InputMessage::Read {}, 13))
        );
    }

    #[test]
    fn deseriaise_input_write_message() {
        assert_eq!(
            from_str::<InputMessage>(
                r#"{
        "messageType" : "Write",
        "id": "test",
        }"#
            ),
            Ok((InputMessage::Write { id: "test" }, 13))
        );
    }
}
