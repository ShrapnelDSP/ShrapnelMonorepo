pub(crate) type Result<T> = core::result::Result<T, ()>;

pub trait Crud<T> {
    /// Create a new entry in the database. Returns the id of the new entry, or
    /// an error when the entry could not be created.
    fn create(&mut self, entry: &T) -> Result<u32>;
    fn read(&mut self, id: u32) -> Result<T>;
    fn update(&mut self, id: u32, entry: &T) -> Result<()>;
    fn destroy(&mut self, id: u32) -> Result<()>;
   
    fn for_each(callback: &mut dyn FnMut(u32, &T));
}