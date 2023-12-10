import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:state_notifier/state_notifier.dart';

part 'presets.freezed.dart';

@freezed
class PresetParametersData with _$PresetParametersData {
  factory PresetParametersData({
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
  }) = _PresetParametersData;
}

@freezed
class PresetRecord with _$PresetRecord {
  factory PresetRecord({
    required int id,
    required PresetState preset,
  }) = _PresetRecord;
}

@freezed
class PresetState with _$PresetState {
  factory PresetState({
    required String name,
    required PresetParametersData parameters,
  }) = _PresetState;
}

@freezed
class PresetsState with _$PresetsState {
  factory PresetsState.loading() = Loading;

  factory PresetsState.ready({
    required bool isCurrentModified,
    required bool canUndo,
    required List<PresetRecord> presets,
    required int? selectedPreset,
  }) = _PresetsState;

  PresetsState._();
}

abstract class PresetsServiceBase extends StateNotifier<PresetsState> {
  PresetsServiceBase(super._state);

  /// Creates a new preset and makes it active.
  void create(String name);

  /// Changes the active preset to a new value.
  void select(int id);

  /// Delete a preset.
  void delete(int id);

  /// Save changes made to the current preset.
  void saveChanges();
}
