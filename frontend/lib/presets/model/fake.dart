import 'dart:async';

import 'package:rxdart/rxdart.dart';
import 'package:state_notifier/state_notifier.dart';

import 'presets.dart';

class FakePresetsModel extends StateNotifier<PresetsState>
    implements PresetsModel {
  FakePresetsModel({
    required ValueStream<PresetParametersState> parametersState,
    required void Function(PresetParametersState) setParametersState,
  })  : _setParametersState = setParametersState,
        _parametersState = parametersState,
        super(PresetsState.loading()) {
    unawaited(
      Future<void>.delayed(const Duration(seconds: 2))
          .then((_) => _updateState()),
    );

    subscription = _parametersState.listen((_) => _updateState);
  }

  final ValueStream<PresetParametersState> _parametersState;
  // This is for faking only, the firmware is responsible for updating
  // parameters when a preset is loaded
  final void Function(PresetParametersState parameters) _setParametersState;
  late final StreamSubscription<PresetParametersState> subscription;

  final _presets = <PresetState>[
    PresetState(
      name: 'Default',
      parameters: PresetParametersState(
        ampGain: 0.5,
        ampChannel: 0,
        bass: 0.5,
        middle: 0.5,
        treble: 0.5,
        contour: 0.5,
        volume: 0.5,
        noiseGateThreshold: 0.5,
        noiseGateHysteresis: 0,
        noiseGateAttack: 0.1,
        noiseGateHold: 0.1,
        noiseGateRelease: 0.1,
        noiseGateBypass: 0,
        chorusRate: 0.95,
        chorusDepth: 0.3,
        chorusMix: 0.8,
        chorusBypass: 1,
        wahPosition: 0.5,
        wahVocal: 0,
        wahBypass: 1,
      ),
    ),
  ];
  int selectedPreset = 0;

  void _updateState() {
    final presetCount = _presets.length;
    state = PresetsState.ready(
      isCurrentModified:
          _parametersState.value != _presets[selectedPreset].parameters,
      canDeleteCurrent: presetCount > 1,
      presets: _presets,
      selectedPreset: selectedPreset,
      canUndo: false,
    );
  }

  @override
  void create(String name) {
    final parameters = _parametersState.value;
    final preset = PresetState(name: name, parameters: parameters);
    _presets.add(preset);
    selectedPreset = _presets.indexOf(preset);
    _updateState();
  }

  @override
  void select(int selected) {
    selectedPreset = selected;
    _setParametersState(_presets[selectedPreset].parameters);
    _updateState();
  }

  @override
  void delete(int indexToDelete) {
    if (indexToDelete == selectedPreset) {
      selectedPreset--;
      if (selectedPreset < 0) {
        selectedPreset = 0;
      }
    }

    _presets.removeAt(indexToDelete);
    _updateState();
  }

  @override
  void saveChanges() {}

  @override
  void dispose() {
    super.dispose();
    unawaited(subscription.cancel());
  }
}
