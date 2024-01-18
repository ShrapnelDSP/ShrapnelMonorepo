pub(crate) type Result<T> = core::result::Result<T, ()>;

pub trait BytesCrud {
    /// Create a new entry in the database. Returns the id of the new entry, or
    /// an error when the entry could not be created.
    fn create(&mut self, entry: &[u8]) -> Result<u32>;
    fn read<'a>(
        &mut self,
        id: u32,
        buffer: &'a mut [u8],
    ) -> Result<&'a mut [u8]>;
    fn update(&mut self, id: u32, entry: &[u8]) -> Result<()>;
    fn delete(&mut self, id: u32) -> Result<()>;
}

pub trait Crud<T> {
    /// Create a new entry in the database. Returns the id of the new entry, or
    /// an error when the entry could not be created.
    fn create(&mut self, entry: &T) -> Result<u32>;
    fn read<'a>(&mut self, id: u32) -> Result<T>;
    fn update(&mut self, id: u32, entry: &T) -> Result<()>;
    fn delete(&mut self, id: u32) -> Result<()>;
}

pub trait CrudIterable<T> {
    fn for_each(callback: &mut dyn FnMut(u32, &T));
}
