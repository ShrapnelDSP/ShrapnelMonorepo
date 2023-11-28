import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:state_notifier/state_notifier.dart';
import 'package:uuid/uuid.dart';

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
class PresetState with _$PresetState {
  factory PresetState({
    required UuidValue id,
    required String name,
    required PresetParametersData parameters,
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
    required UuidValue selectedPreset,
  }) = _PresetsState;

  PresetsState._();
}

abstract class PresetsServiceBase extends StateNotifier<PresetsState> {
  PresetsServiceBase(super._state);

  /// Creates a new preset and makes it active.
  void create(String name);

  /// Changes the active preset to a new value.
  void select(UuidValue id);

  /// Delete a preset.
  void delete(UuidValue id);

  /// Save changes made to the current preset.
  void saveChanges();
}