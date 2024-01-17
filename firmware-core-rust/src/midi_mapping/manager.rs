use super::Mapping;
use crate::midi_mapping::MappingMode;
use crate::midi_protocol::{MidiMessage, MidiMessageParameters, CC_VALUE_MAX};
use crate::parameters::ParameterId;
use core::ops::Index;
use heapless::LinearMap;
use mockall::automock;

use crate::persistence;

// TODO make this observable

/// React to a MIDI message by updating an audio parameter if there is a
/// mapping registered
fn process_mapped_message<MapType: for<'a> Index<&'a u32, Output = Mapping>>(
    mappings: &MapType,
    parameters: &mut impl ParameterSetter,
    message: MidiMessage,
) where
    for<'b> &'b MapType: IntoIterator<Item = (&'b u32, &'b Mapping)>,
{
    let MidiMessageParameters::ControlChange(cc_params) = message.parameters
    else {
        return;
    };

    for (_, mapping) in mappings {
        if mapping.midi_channel != message.channel {
            continue;
        }

        if mapping.cc_number != cc_params.control {
            continue;
        }

        match mapping.mode {
            MappingMode::Parameter => parameters.update(
                &mapping.parameter_name,
                cc_params.value as f32 / CC_VALUE_MAX as f32,
            ),
            MappingMode::Toggle => {
                if cc_params.value == 0 {
                    continue;
                }

                let old_value = parameters.get(&mapping.parameter_name);
                parameters.update(
                    &mapping.parameter_name,
                    if old_value < 0.5 { 1. } else { 0. },
                );
            }
        }
    }
}

#[cfg_attr(test, automock)]
pub(crate) trait ParameterSetter {
    fn update(&mut self, parameter_id: &ParameterId, value: f32);
    fn get(&mut self, parameter_id: &ParameterId) -> f32;
}

struct MidiMappingManager<
    Crud: for<'a> persistence::Crud<&'a [u8]>,
    const N: usize,
> {
    storage: Crud,
    mappings: LinearMap<u32, Mapping, N>,
}

impl<Crud: for<'a> persistence::Crud<&'a [u8]>, const N: usize>
    MidiMappingManager<Crud, N>
{
    fn new(storage: Crud) -> Self {
        MidiMappingManager {
            storage,
            mappings: LinearMap::default(),
        }
    }

    fn get_mappings(&self) -> &LinearMap<u32, Mapping, N> {
        &self.mappings
    }
}

impl<Crud, const N: usize> persistence::Crud<Mapping>
    for MidiMappingManager<Crud, N>
where
    Crud: for<'a> persistence::Crud<&'a [u8]>,
{
    fn create(&mut self, entry: &Mapping) -> persistence::crud::Result<u32> {
        todo!()
    }

    fn read<'a>(
        &mut self,
        id: u32,
        entry_out: &'a mut Mapping,
    ) -> persistence::crud::Result<()> {
        todo!()
    }

    fn update(
        &mut self,
        id: u32,
        entry: &Mapping,
    ) -> persistence::crud::Result<()> {
        todo!()
    }

    fn destroy(&mut self, id: u32) -> persistence::crud::Result<()> {
        todo!()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::midi_protocol::ControlChange;
    use crate::persistence::crud::MockCrud;
    use fstr::FStr;
    use mockall::{predicate, Sequence};
    use std::sync::{Arc, Mutex};

    #[test]
    fn wrong_channel_message_is_ignored() {
        let mut mappings: LinearMap<u32, Mapping, 4> = LinearMap::default();
        mappings
            .insert(
                0,
                Mapping {
                    midi_channel: 1,
                    mode: MappingMode::Parameter,
                    cc_number: 42,
                    parameter_name: ParameterId(FStr::from_str_lossy(
                        "test", 0,
                    )),
                },
            )
            .expect("setup failed");

        let mut mock_parameters = MockParameterSetter::default();
        process_mapped_message(
            &mappings,
            &mut mock_parameters,
            MidiMessage {
                channel: 0,
                parameters: MidiMessageParameters::ControlChange(
                    ControlChange {
                        control: 42,
                        value: 0,
                    },
                ),
            },
        )
    }

    #[test]
    fn wrong_controller_message_is_ignored() {
        let mut mappings: LinearMap<u32, Mapping, 4> = LinearMap::default();
        mappings
            .insert(
                0,
                Mapping {
                    midi_channel: 1,
                    mode: MappingMode::Parameter,
                    cc_number: 42,
                    parameter_name: ParameterId(FStr::from_str_lossy(
                        "test", 0,
                    )),
                },
            )
            .expect("setup failed");

        let mut mock_parameters = MockParameterSetter::default();
        process_mapped_message(
            &mappings,
            &mut mock_parameters,
            MidiMessage {
                channel: 1,
                parameters: MidiMessageParameters::ControlChange(
                    ControlChange {
                        control: 0,
                        value: 0,
                    },
                ),
            },
        )
    }

    #[test]
    fn parameter_mode_works() {
        let mut mappings: LinearMap<u32, Mapping, 4> = LinearMap::default();
        mappings
            .insert(
                0,
                Mapping {
                    midi_channel: 1,
                    mode: MappingMode::Parameter,
                    cc_number: 42,
                    parameter_name: ParameterId(FStr::from_str_lossy(
                        "test", 0,
                    )),
                },
            )
            .expect("setup failed");

        let mut mock_parameters = MockParameterSetter::default();
        mock_parameters
            .expect_update()
            .with(
                predicate::eq(ParameterId(FStr::from_str_lossy("test", 0))),
                predicate::eq(1.),
            )
            .once()
            .returning(|_, __| ());
        process_mapped_message(
            &mappings,
            &mut mock_parameters,
            MidiMessage {
                channel: 1,
                parameters: MidiMessageParameters::ControlChange(
                    ControlChange {
                        control: 42,
                        value: 127,
                    },
                ),
            },
        )
    }

    #[test]
    fn toggle_mode_works() {
        let mut mappings: LinearMap<u32, Mapping, 4> = LinearMap::default();
        mappings
            .insert(
                0,
                Mapping {
                    midi_channel: 1,
                    mode: MappingMode::Toggle,
                    cc_number: 42,
                    parameter_name: ParameterId(FStr::from_str_lossy(
                        "test", 0,
                    )),
                },
            )
            .expect("setup failed");

        let fake_value = Arc::new(Mutex::new(0.5));
        let mut mock_parameters = MockParameterSetter::default();

        let fake_value_clone = Arc::clone(&fake_value);
        mock_parameters
            .expect_get()
            .with(predicate::eq(ParameterId(FStr::from_str_lossy("test", 0))))
            .returning(move |_| *fake_value_clone.lock().unwrap());

        let mut seq = Sequence::default();

        let fake_value_clone = Arc::clone(&fake_value);
        mock_parameters
            .expect_update()
            .with(
                predicate::eq(ParameterId(FStr::from_str_lossy("test", 0))),
                predicate::eq(0.),
            )
            .once()
            .in_sequence(&mut seq)
            .returning(move |_, value| {
                *fake_value_clone.lock().unwrap() = value;
            });

        let fake_value_clone = Arc::clone(&fake_value);
        mock_parameters
            .expect_update()
            .with(
                predicate::eq(ParameterId(FStr::from_str_lossy("test", 0))),
                predicate::eq(1.),
            )
            .once()
            .in_sequence(&mut seq)
            .returning(move |_, value| {
                *fake_value_clone.lock().unwrap() = value;
            });

        // this mode ignores messages with value 0, and any message with
        // non-zero value will switch the parameter value between 0 and 1.
        process_mapped_message(
            &mappings,
            &mut mock_parameters,
            MidiMessage {
                channel: 1,
                parameters: MidiMessageParameters::ControlChange(
                    ControlChange {
                        control: 42,
                        value: 0,
                    },
                ),
            },
        );

        process_mapped_message(
            &mappings,
            &mut mock_parameters,
            MidiMessage {
                channel: 1,
                parameters: MidiMessageParameters::ControlChange(
                    ControlChange {
                        control: 42,
                        value: 1,
                    },
                ),
            },
        );

        process_mapped_message(
            &mappings,
            &mut mock_parameters,
            MidiMessage {
                channel: 1,
                parameters: MidiMessageParameters::ControlChange(
                    ControlChange {
                        control: 42,
                        value: 1,
                    },
                ),
            },
        );
    }

    fn create() {
        let mock_storage = MockCrud::default();
        let mut sut = MidiMappingManager::new(mock_storage);
        assert_eq!(sut.get_mappings().len(), 0);
    }

    // reference c++ tests
}
