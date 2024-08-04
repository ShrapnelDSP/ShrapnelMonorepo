/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

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
