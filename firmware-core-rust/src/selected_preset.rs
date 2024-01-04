pub(crate) struct PresetId(u32);

pub(crate) enum ApiMessage {
    Read,
    Notify { selected_preset_id: PresetId },
    Write { selected_preset_id: PresetId },
}
