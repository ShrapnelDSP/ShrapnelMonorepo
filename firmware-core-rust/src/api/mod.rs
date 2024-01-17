/// Coder decoder from T to bytes
pub trait BytesCodec<T> {
    fn from_bytes(bytes: &[u8]) -> Option<T>;
    fn to_bytes(self: &Self, entry_out: &mut [u8]) -> ();
}
