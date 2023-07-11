import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:state_notifier/state_notifier.dart';

part 'presets.freezed.dart';

@freezed
class PresetParametersState with _$PresetParametersState {
  factory PresetParametersState({
    required double ampGain,
    required double ampChannel,
    required double bass,
    required double middle,
    required double treble,
    required double contour,
    required double volume,
    required double noiseGateThreshold,
    required double noiseGateHysteresis,
    required double noiseGateAttack,
    required double noiseGateHold,
    required double noiseGateRelease,
    required double noiseGateBypass,
    required double chorusRate,
    required double chorusDepth,
    required double chorusMix,
    required double chorusBypass,
    required double wahPosition,
    required double wahVocal,
    required double wahBypass,
  }) = _PresetParametersState;
}

@freezed
class PresetState with _$PresetState {
  factory PresetState({
    required String name,
    required PresetParametersState parameters,
  }) = _PresetState;
}

@freezed
class PresetsState with _$PresetsState {
  factory PresetsState.loading() = Loading;

  factory PresetsState.ready({
    required bool isCurrentModified,
    required bool canDeleteCurrent,
    required bool canUndo,
    required List<PresetState> presets,
    required int selectedPreset,
  }) = _PresetsState;

  PresetsState._();

  bool get isReady => maybeMap(ready: (_) => true, orElse: () => false);
}

abstract class PresetsModel extends StateNotifier<PresetsState> {
  PresetsModel(super.state);

  /// Creates a new preset and makes it active.
  void create(String name);

  /// Changes the active preset to a new value.
  void select(int indexToSelect);

  /// Delete a preset.
  void delete(int indexToDelete);

  /// Save changes made to the current preset.
  void saveChanges();

  /// Undo the last operation performed on a preset.
  void undo();
}
