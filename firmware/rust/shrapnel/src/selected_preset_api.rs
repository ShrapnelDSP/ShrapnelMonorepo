// FIXME: set up namespacing correctly
//mod shrapnel {
//mod selected_preset {

use core::fmt::Formatter;
use serde::de::{MapAccess, Visitor};
use serde::{Deserialize, Deserializer};
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
#[derive(Debug, PartialEq)]
pub enum InputMessage<'a> {
    /// The client wants to get the current value of the selected preset
    Read {},
    /// The client wants to set the current value of the selected preset
    Write { id: &'a str },
}

struct InputMessageVisitor {}

impl Visitor for InputMessageVisitor {
    type Value = InputMessage;

    fn expecting(&self, formatter: &mut Formatter) -> core::fmt::Result {
        formatter.write_str("FIXME")
    }

    fn visit_map<A>(self, map: A) -> Result<Self::Value, A::Error>
    where
        A: MapAccess,
    {
        let tmp = heapless::FnvIndexMap<heapless::String<16>>;
        map.size_hint()
    }
}

impl Deserialize for InputMessage {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_map();
    }
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
