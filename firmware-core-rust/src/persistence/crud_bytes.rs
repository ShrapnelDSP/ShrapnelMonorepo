use super::Crud;
use std::iter::Map;
use std::vec::Vec;

struct FakeCrudBytes {
    next_id: i32,
    storage: Map<i32, Vec<u8>>,
}

impl<'a> Crud<&'a [u8]> for FakeCrudBytes {
    fn create(
        &mut self,
        entry: &&[u8],
    ) -> crate::persistence::crud::Result<u32> {
        todo!()
    }

    fn read<'b>(
        &mut self,
        id: u32,
        entry_out: &'b mut &'a [u8],
    ) -> crate::persistence::crud::Result<&'b mut &'a [u8]> {
        todo!()
    }

    fn update(
        &mut self,
        id: u32,
        entry: &&[u8],
    ) -> crate::persistence::crud::Result<()> {
        todo!()
    }

    fn destroy(&mut self, id: u32) -> crate::persistence::crud::Result<()> {
        todo!()
    }
}
