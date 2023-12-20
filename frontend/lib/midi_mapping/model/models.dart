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

// ignore_for_file: invalid_annotation_target
// freezed and JsonKey generate this warning when used together correctly

import 'package:freezed_annotation/freezed_annotation.dart';

part 'models.freezed.dart';

@freezed
class MidiApiMessage with _$MidiApiMessage {
  const factory MidiApiMessage.getRequest() = GetRequest;
  const factory MidiApiMessage.getResponse({
    required Map<String, MidiMapping> mappings,
  }) = GetResponse;
  const factory MidiApiMessage.createRequest({
    required MidiMappingEntry mapping,
  }) = CreateRequest;
  const factory MidiApiMessage.createResponse({
    required MidiMappingEntry mapping,
  }) = CreateResponse;
  const factory MidiApiMessage.update({
    required MidiMappingEntry mapping,
  }) = Update;
  const factory MidiApiMessage.remove({required String id}) = Remove;
  const factory MidiApiMessage.midiMessageReceived({
    required MidiMessage message,
  }) = MidiMessageReceived;
}

@freezed
class MidiMapping with _$MidiMapping {
  const factory MidiMapping({
    @JsonKey(name: 'midi_channel') required int midiChannel,
    @JsonKey(name: 'cc_number') required int ccNumber,
    @JsonKey(name: 'parameter_id') required String parameterId,
    @JsonKey(name: 'mode') required MidiMappingMode mode,
  }) = _MidiMapping;

  const MidiMapping._();
}

@freezed
class MidiMappingEntry with _$MidiMappingEntry {
  const factory MidiMappingEntry({
    required String id,
    required MidiMapping mapping,
  }) = _MidiMappingEntry;

  const MidiMappingEntry._();
}

enum MidiMappingMode {
  toggle(apiName: 'toggle', uiName: 'Toggle'),
  parameter(apiName: 'parameter', uiName: 'Knob');

  const MidiMappingMode({required this.apiName, required this.uiName});

  final String apiName;
  final String uiName;
}

@freezed
class MidiMessage with _$MidiMessage {
  const factory MidiMessage.noteOn({
    required int channel,
    required int note,
    required int velocity,
  }) = _NoteOn;
  const factory MidiMessage.noteOff({
    required int channel,
    required int note,
    required int velocity,
  }) = _NoteOff;
  const factory MidiMessage.controlChange({
    required int channel,
    required int control,
    required int value,
  }) = _ControlChange;
  const factory MidiMessage.programChange({
    required int channel,
    required int number,
  }) = _ProgramChange;
}
