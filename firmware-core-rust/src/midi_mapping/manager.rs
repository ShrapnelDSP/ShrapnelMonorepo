use core::ops::Index;
use heapless::LinearMap;
use crate::midi_mapping::MappingMode;
use crate::midi_protocol::{CC_VALUE_MAX, MidiMessage, MidiMessageParameters};
use crate::parameters::ParameterId;
use super::Mapping;

use crate::persistence;

// TODO make this observable

fn process_mapped_message<MapType: Index<u32, Output=Mapping> + IntoIterator<Item=(u32, Mapping)>>(mappings: MapType, mut parameters: impl ParameterSetter, message: MidiMessage)
{
    let MidiMessageParameters::ControlChange(cc_params) = message.parameters else {
        return;
    };

    for (_, mapping) in mappings {
        if mapping.midi_channel != message.channel
        {
            continue;
        }

        if mapping.cc_number != cc_params.control
        {
            continue;
        }

        match mapping.mode {
            MappingMode::Parameter =>
                parameters.update(&mapping.parameter_name, cc_params.value as f32 / CC_VALUE_MAX as f32),
            MappingMode::Toggle => {
                if cc_params.value == 0
                {
                    continue;
                }

                let old_value = parameters.get(&mapping.parameter_name);
                parameters.update(&mapping.parameter_name, if old_value < 0.5 { 1. } else { 0. });
            }
        }
    }
}

pub(crate) trait ParameterSetter {
    fn update(&mut self, parameter_id: &ParameterId, value: f32);
    fn get(&mut self, parameter_id: &ParameterId) -> f32;
}

struct MidiMappingManager<Crud: for<'a> persistence::Crud<&'a [u8]>, const N: usize> {
    storage: Crud,
    mappings: LinearMap<u32, Mapping, N>,
}

impl<Crud: for<'a> persistence::Crud<&'a [u8]>, const N: usize> MidiMappingManager<Crud, N> {
    fn get_mappings(&self) -> &LinearMap<u32, Mapping, N> {
        &self.mappings
    }
}

impl<Crud: for<'a> persistence::Crud<&'a [u8]>, const N: usize> persistence::Crud<Mapping> for MidiMappingManager<Crud, N> {
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