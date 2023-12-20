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

part 'models.freezed.dart';

@freezed
sealed class MidiApiMessage with _$MidiApiMessage {
  const factory MidiApiMessage.getRequest() = MidiGetRequest;

  const factory MidiApiMessage.getResponse({
    required Map<String, MidiMapping> mappings,
  }) = MidiGetResponse;

  const factory MidiApiMessage.createRequest({
    required MidiMappingEntry mapping,
  }) = MidiCreateRequest;

  const factory MidiApiMessage.createResponse({
    required MidiMappingEntry mapping,
  }) = MidiCreateResponse;

  const factory MidiApiMessage.update({
    required MidiMappingEntry mapping,
  }) = MidiUpdate;

  const factory MidiApiMessage.remove({required String id}) = MidiRemove;

  const factory MidiApiMessage.midiMessageReceived({
    required MidiMessage message,
  }) = MidiMessageReceived;
}

@freezed
class MidiMapping with _$MidiMapping {
  const factory MidiMapping({
    required int midiChannel,
    required int ccNumber,
    required String parameterId,
    required MidiMappingMode mode,
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
  toggle(uiName: 'Toggle'),
  parameter(uiName: 'Knob');

  const MidiMappingMode({required this.uiName});

  final String uiName;
}

@freezed
class MidiMessage with _$MidiMessage {
  const factory MidiMessage.noteOn({
    required int channel,
    required int note,
    required int velocity,
  }) = MidiMessageNoteOn;

  const factory MidiMessage.noteOff({
    required int channel,
    required int note,
    required int velocity,
  }) = MidiMessageNoteOff;

  const factory MidiMessage.controlChange({
    required int channel,
    required int control,
    required int value,
  }) = MidiMessageControlChange;

  const factory MidiMessage.programChange({
    required int channel,
    required int number,
  }) = MidiMessageProgramChange;
}
