import 'dart:async';

import 'package:state_notifier/state_notifier.dart';

import 'presets.dart';

class FakePresetsModel extends StateNotifier<PresetsState>
    implements PresetsModel {
  FakePresetsModel({
    required PresetParametersState Function() getParametersState,
    required void Function(PresetParametersState) setParametersState,
  })  : _setParametersState = setParametersState,
        _getParametersState = getParametersState,
        super(PresetsState.loading()) {
    unawaited(
      Future<void>.delayed(const Duration(seconds: 2))
          .then((_) => _updateState()),
    );
  }

  final PresetParametersState Function() _getParametersState;
  // This is for faking only, the firmware is responsible for updating
  // parameters when a preset is loaded
  final void Function(PresetParametersState parameters) _setParametersState;

  final _presets = <PresetState>[
    PresetState(
      name: 'a',
      parameters: PresetParametersState(
        ampGain: 0.5,
        ampChannel: 0,
        bass: 0.5,
        middle: 0.5,
        treble: 0.5,
        contour: 0.5,
        volume: -15,
        noiseGateThreshold: -60,
        noiseGateHysteresis: 0,
        noiseGateAttack: 10,
        noiseGateHold: 50,
        noiseGateRelease: 50,
        noiseGateBypass: 0,
        chorusRate: 0.95,
        chorusDepth: 0.3,
        chorusMix: 0.8,
        chorusBypass: 1,
        wahPosition: 0.5,
        wahVocal: 0,
        wahBypass: 1,
      ),
    )
  ];
  int selectedPreset = 0;

  void _updateState() {
    final presetCount = _presets.length;
    state = PresetsState.ready(
      isCurrentModified: (presetCount & 1) != 0,
      canDeleteCurrent: presetCount > 1,
      presets: _presets,
      selectedPreset: selectedPreset,
      canUndo: false,
    );
  }

  @override
  void create(String name) {
    final parameters = _getParametersState();
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
  void undo() {}
}
