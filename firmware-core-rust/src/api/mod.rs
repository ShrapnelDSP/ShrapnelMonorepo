/// Coder decoder from T to bytes
pub trait BytesCodec<T> {
    fn decode(bytes: &[u8]) -> Option<T>;
    fn encode<'a>(message: &T, entry_out: &'a mut [u8]) -> Option<&'a [u8]>;
}
