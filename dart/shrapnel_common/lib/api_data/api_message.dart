import 'package:freezed_annotation/freezed_annotation.dart';

import 'audio_event_message.dart';
import 'midi_api_message.dart';
import 'parameter_message.dart';
import 'presets_message.dart';
import 'selected_preset_data.dart';

part 'api_message.freezed.dart';

@freezed
sealed class ApiMessage with _$ApiMessage {
  const factory ApiMessage.audioEvent({required AudioEventMessage message}) =
      ApiMessageAudioEvent;

  const factory ApiMessage.parameterInput({
    required ParameterServiceInputMessage message,
  }) = ApiMessageParameterInput;

  const factory ApiMessage.parameterOutput({
    required ParameterServiceOutputMessage message,
  }) = ApiMessageParameterOutput;

  const factory ApiMessage.midiMapping({required MidiApiMessage message}) =
      ApiMessageMidiMapping;

  const factory ApiMessage.presets({required PresetsMessage message}) =
      ApiMessagePresets;

  const factory ApiMessage.selectedPreset({
    required SelectedPresetMessage message,
  }) = ApiMessageSelectedPreset;
}
