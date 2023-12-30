use heapless::LinearMap;
use super::Mapping;
use crate::persistence::Crud;

// TODO make this observable
// refactor to remove get method. Why is it used when we can iterate?

struct MidiMappingManager<const N: usize> {
    parameters: Arc</* TODO parameters manager */>,
    storage: Arc<dyn Crud<[u8]>>,
    mappings: LinearMap<u32, Mapping, N>,
}

impl<const N: usize> Crud<Mapping> for MidiMappingManager<N> {
    fn create(&mut self, entry: &Mapping) -> crate::persistence::crud::Result<u32> {
        todo!()
    }

    fn read(&mut self, id: u32) -> crate::persistence::crud::Result<Mapping> {
        todo!()
    }

    fn update(&mut self, id: u32, entry: &Mapping) -> crate::persistence::crud::Result<()> {
        todo!()
    }

    fn destroy(&mut self, id: u32) -> crate::persistence::crud::Result<()> {
        todo!()
    }

    fn for_each(callback: &mut dyn FnMut(u32, &Mapping)) {
        todo!()
    }
}