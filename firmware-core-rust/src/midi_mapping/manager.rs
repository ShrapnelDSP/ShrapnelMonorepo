use super::Mapping;
use crate::api::BytesCodec;
use crate::midi_mapping::MappingMode;
use crate::midi_protocol::{MidiMessage, MidiMessageParameters, CC_VALUE_MAX};
use crate::parameters::ParameterId;
use core::ops::Index;
use heapless::LinearMap;
use mockall::automock;
use std::marker::PhantomData;

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
    Codec: BytesCodec<Mapping>,
    const N: usize,
> {
    storage: Crud,
    mappings: LinearMap<u32, Mapping, N>,
    codec: PhantomData<Codec>,
}

impl<
        Crud: for<'a> persistence::Crud<&'a [u8]>,
        Codec: BytesCodec<Mapping>,
        const N: usize,
    > MidiMappingManager<Crud, Codec, N>
{
    fn new(storage: Crud) -> Self {
        MidiMappingManager {
            storage,
            mappings: LinearMap::default(),
            codec: PhantomData::default(),
        }
    }

    fn get_mappings(&self) -> &LinearMap<u32, Mapping, N> {
        &self.mappings
    }
}

impl<Crud, Codec: BytesCodec<Mapping>, const N: usize>
    persistence::Crud<Mapping> for MidiMappingManager<Crud, Codec, N>
where
    Crud: for<'a> persistence::Crud<&'a [u8]>,
{
    fn create(&mut self, entry: &Mapping) -> persistence::crud::Result<u32> {
        let mut memory = [0u8; 256];

        Codec::encode(entry, &mut memory);

        if self.mappings.len() == self.mappings.capacity() {
            return Err(());
        }

        let Ok(id) = self.storage.create(&[].as_slice()) else {
            return Err(());
        };

        // Only fails if the map is full, but we already checked for that
        self.mappings
            .insert(id, entry.clone())
            .expect("Failed to insert mapping");

        // TODO notify new key added

        return Ok(id);
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
        if !self.mappings.contains_key(&id) {
            return Err(());
        }

        let mut memory = [0u8; 256];

        let Some(encoded) = Codec::encode(entry, &mut memory) else {
            return Err(());
        };

        let Ok(_) = self.storage.update(id, &encoded) else {
            return Err(());
        };

        self.mappings
            .insert(id, entry.clone())
            .expect("should not fail when updating an existing element");

        // TODO notify observers about updated id

        return Ok(());
    }

    fn destroy(&mut self, id: u32) -> persistence::crud::Result<()> {
        todo!()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::midi_protocol::ControlChange;
    use crate::persistence::crud::Result;
    use crate::persistence::Crud;
    use fstr::FStr;
    use mockall::{mock, predicate, Sequence};
    use std::sync::{Arc, Mutex};

    mod process {
        use super::*;

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
                .with(predicate::eq(ParameterId(FStr::from_str_lossy(
                    "test", 0,
                ))))
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
    }

    mod manager {
        use super::*;
        use std::println;

        mock! {
           BytesCrud {}
           impl Crud<&[u8]> for BytesCrud {
            fn create<'a>(&mut self, entry: & &'a [u8]) -> Result<u32>;
            fn read<'a>(&mut self, id: u32, entry_out: &mut &'a[u8]) -> Result<()>;
            fn update<'a>(&mut self, id: u32, entry: & &'a [u8]) -> Result<()>;
            fn destroy(&mut self, id: u32) -> Result<()>;
           }
        }

        struct MockMappingCodec {}

        impl BytesCodec<Mapping> for MockMappingCodec {
            fn decode(bytes: &[u8]) -> Option<Mapping> {
                todo!()
            }

            fn encode<'a>(
                message: &Mapping,
                entry_out: &'a mut [u8],
            ) -> Option<&'a [u8]> {
                Some(&entry_out[0..10])
            }
        }

        #[test]
        fn create() {
            let mut mock_storage = MockBytesCrud::default();
            let mut i = 0;
            mock_storage.expect_create().returning(move |_| {
                i += 1;
                Ok(i)
            });

            let mut sut: MidiMappingManager<
                MockBytesCrud,
                MockMappingCodec,
                2,
            > = MidiMappingManager::new(mock_storage);

            assert_eq!(sut.get_mappings().len(), 0);

            let mapping = Mapping {
                midi_channel: 1,
                cc_number: 2,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("gain", 0)),
            };
            sut.create(&mapping).unwrap();
            assert_eq!(sut.get_mappings().len(), 1);

            let mapping = Mapping {
                midi_channel: 3,
                cc_number: 4,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("volume", 0)),
            };
            sut.create(&mapping).unwrap();
            assert_eq!(sut.get_mappings().len(), 2);

            let mapping = Mapping {
                midi_channel: 5,
                cc_number: 6,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("tone", 0)),
            };
            assert!(sut.create(&mapping).is_err());
            assert_eq!(sut.get_mappings().len(), 2);
        }

        #[test]
        fn update() {
            let mut mock_storage = MockBytesCrud::default();
            let mut i = 0;
            mock_storage.expect_create().returning(move |_| {
                i += 1;
                Ok(i)
            });
            mock_storage.expect_update().returning(|_, _| Ok(()));

            let mut sut: MidiMappingManager<
                MockBytesCrud,
                MockMappingCodec,
                2,
            > = MidiMappingManager::new(mock_storage);

            assert_eq!(sut.get_mappings().len(), 0);

            let mapping = Mapping {
                midi_channel: 1,
                cc_number: 2,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("gain", 0)),
            };
            let id = sut.create(&mapping).unwrap();

            let mapping = Mapping {
                midi_channel: 7,
                cc_number: 8,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("contour", 0)),
            };
            assert!(
                sut.update(3, &mapping).is_err(),
                "update non-existent should fail"
            );
            assert_eq!(sut.get_mappings().len(), 1);

            let mapping = Mapping {
                midi_channel: 3,
                cc_number: 4,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("volume", 0)),
            };
            sut.create(&mapping).unwrap();
            assert_eq!(sut.get_mappings().len(), 2);

            let mapping = Mapping {
                midi_channel: 5,
                cc_number: 6,
                mode: MappingMode::Parameter,
                parameter_name: ParameterId(FStr::from_str_lossy("tone", 0)),
            };
            sut.update(id, &mapping).unwrap();

            // TODO verify the mapping getter returns expected mappings
            println!("{:?}", sut.get_mappings());
        }
    }

    // reference c++ tests
}
