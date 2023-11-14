import 'package:rxdart/rxdart.dart';
import 'package:uuid/uuid.dart';

import 'presets.dart';
import 'presets_service.dart';

// TODO define some lower level APIs used to implement the repositories here
// Need to define the low level types for the API, and how notifications are
// going to work.

class PresetsRepository implements PresetsRepositoryBase {
  @override
  Future<void> create(PresetState preset) {
    // TODO: implement create
    throw UnimplementedError();
  }

  @override
  Future<void> delete(UuidValue id) {
    // TODO: implement delete
    throw UnimplementedError();
  }

  @override
  // TODO: implement presets
  ValueStream<Map<UuidValue, PresetState>> get presets =>
      throw UnimplementedError();

  @override
  Future<void> update(PresetState preset) {
    // TODO: implement update
    throw UnimplementedError();
  }
}

class SelectedPresetRepository implements SelectedPresetRepositoryBase {
  @override
  Future<void> selectPreset(UuidValue presetId) {
    // TODO: implement load
    throw UnimplementedError();
  }

  @override
  // TODO: implement selectedPreset
  ValueStream<UuidValue> get selectedPreset => throw UnimplementedError();
}
