import 'dart:async';

import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';
import 'package:state_notifier/state_notifier.dart';
import 'package:uuid/uuid.dart';

import '../../util/uuid.dart';
import 'presets.dart';

final _log = Logger('presets_fake');

abstract class PresetsRepositoryBase {
  Future<void> create(PresetState preset);

  // No need for read, it will be done internally then subscribed. Notifications
  // are implicit from the user's perspective.
  // Future<void> read(String id);
  Future<void> update(PresetState preset);

  Future<void> delete(UuidValue id);

  ValueStream<Map<UuidValue, PresetState>> get presets;
}

abstract class SelectedPresetRepositoryBase {
  /// Load an existing preset.
  ///
  /// [id] must be the ID of one of the existing presets.
  Future<void> selectPreset(UuidValue id);

  ValueStream<UuidValue> get selectedPreset;
}

// states:
// - initialising/loading
// - running
//
// actions:
// - start
// - create new preset
// - save changes to current preset
// - load existing preset
// - delete preset (delete selected and load the previous or next)
// - parameters changed

class _PresetsStateMachine {}

class PresetsService extends StateNotifier<PresetsState>
    implements PresetsServiceBase {
  PresetsService({
    required this.presetsRepository,
    required this.selectedPresetRepository,
    required this.uuid,
    required ValueStream<PresetParametersData> parametersState,
  })  : _parametersState = parametersState,
        super(PresetsState.loading()) {
    _subscription = _parametersState.listen((_) => _updateState);
  }

  final ValueStream<PresetParametersData> _parametersState;
  late final StreamSubscription<PresetParametersData> _subscription;
  final PresetsRepositoryBase presetsRepository;
  final SelectedPresetRepositoryBase selectedPresetRepository;
  final UuidService uuid;

  void _updateState() {
    _log.finest('_updateState');
    final presets = presetsRepository.presets.valueOrNull;
    final selectedPreset = selectedPresetRepository.selectedPreset.valueOrNull;

    if (selectedPreset == null || presets == null) {
      state = PresetsState.loading();
    } else {
      final presetCount = presets.length;
      final sortedPresets = presets.values.toList()
        ..sort((a, b) => a.name.compareTo(b.name));
      state = PresetsState.ready(
        isCurrentModified:
            _parametersState.value != presets[selectedPreset]?.parameters,
        canDeleteCurrent: presetCount > 1,
        presets: sortedPresets,
        selectedPreset: selectedPreset,
        canUndo: false,
      );
    }
  }

  @override
  Future<void> create(String name) async {
    final id = uuid.v4Value();
    await presetsRepository.create(
      PresetState(id: id, name: name, parameters: _parametersState.value),
    );
    await selectedPresetRepository.selectPreset(id);
  }

  @override
  Future<void> select(UuidValue id) async {
    await selectedPresetRepository.selectPreset(id);
  }

  @override
  Future<void> delete(UuidValue id) async {
    await presetsRepository.delete(id);
  }

  @override
  Future<void> saveChanges() async {
    final currentPresetId = selectedPresetRepository.selectedPreset.valueOrNull;
    if (currentPresetId == null) {
      _log.severe("Don't know current preset id");
      return;
    }

    final currentPreset = presetsRepository.presets.value[currentPresetId];
    if (currentPreset == null) {
      _log.severe("Don't know preset with id $currentPresetId");
      return;
    }

    await presetsRepository.update(
      PresetState(
        id: currentPresetId,
        name: currentPreset.name,
        parameters: _parametersState.value,
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
    unawaited(_subscription.cancel());
  }
}
