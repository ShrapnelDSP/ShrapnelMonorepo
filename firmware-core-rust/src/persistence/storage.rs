use fstr::FStr;

type Result<T> = core::result::Result<T, ()>;

pub trait Storage {
    fn save_bytes(key: FStr<15>, data: &[u8]) -> Result<()>;
    fn save_string(key: FStr<15>, data: &[char]) -> Result<()>;
    fn save_u32(key: FStr<15>, data: u32) -> Result<()>;
    fn save_f32(key: FStr<15>, data: f32) -> Result<()>;

    fn load_bytes(key: FStr<15>, data: &mut [u8]) -> Result<()>;
    fn load_string(key: FStr<15>, data: &mut [char]) -> Result<()>;
    fn load_u32(key: FStr<15>) -> Result<u32>;
    fn load_f32(key: FStr<15>) -> Result<f32>;
}
