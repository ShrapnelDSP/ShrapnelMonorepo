import 'package:freezed_annotation/freezed_annotation.dart';

part 'selected_preset_data.freezed.dart';

@freezed
sealed class SelectedPresetMessage with _$SelectedPresetMessage {
  factory SelectedPresetMessage.read() = ReadSelectedPresetMessage;

  factory SelectedPresetMessage.notify({required int selectedPreset}) =
      NotifySelectedPresetMessage;

  factory SelectedPresetMessage.write({required int selectedPreset}) =
      WriteSelectedPresetMessage;
}
