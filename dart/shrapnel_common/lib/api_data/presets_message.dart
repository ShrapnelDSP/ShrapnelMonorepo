import 'package:freezed_annotation/freezed_annotation.dart';

part 'presets_message.freezed.dart';

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
sealed class PresetsMessage with _$PresetsMessage {
  factory PresetsMessage.initialise() = PresetsMessageInitialise;

  factory PresetsMessage.notify(int id, PresetData preset) =
      PresetsMessageNotify;

  factory PresetsMessage.create(PresetData preset) = PresetsMessageCreate;

  factory PresetsMessage.update(int id, PresetData preset) =
      PresetsMessageUpdate;

  factory PresetsMessage.delete(int id) = PresetsMessageDelete;
}

@freezed
class PresetData with _$PresetData {
  factory PresetData({
    required String name,
    required PresetParametersData parameters,
  }) = _PresetData;

}
