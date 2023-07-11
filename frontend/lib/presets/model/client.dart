import 'package:rxdart/rxdart.dart';

import 'presets.dart';

abstract class PresetsApiClient {
  Future<void> create(PresetState preset);
  // No need for read, it will be done internally then subscribed. Notifications
  // are implicit from the user's perspective.
  // Future<void> read(String id);
  Future<void> update(PresetState preset);
  Future<void> delete(String id);

  Stream<PresetsState> get presets;
}

abstract class SelectedPresetApiClient {
  /// Load an existing preset.
  ///
  /// [presetId] must be the ID of one of the existing presets.
  Future<void> load(String presetId);

  ValueStream<String> get selectedPreset;
}
