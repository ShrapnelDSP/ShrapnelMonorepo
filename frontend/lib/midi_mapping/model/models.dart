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
part 'models.g.dart';

@Freezed(unionKey: 'messageType')
class MidiApiMessage with _$MidiApiMessage {
  @FreezedUnionValue('MidiMap::get::request')
  const factory MidiApiMessage.getRequest() = GetRequest;
  @FreezedUnionValue('MidiMap::get::response')
  const factory MidiApiMessage.getResponse({
    required Map<String, MidiMapping> mappings,
  }) = GetResponse;
  @FreezedUnionValue('MidiMap::create::request')
  const factory MidiApiMessage.createRequest({
    required MidiMappingEntry mapping,
  }) = CreateRequest;
  @FreezedUnionValue('MidiMap::create::response')
  const factory MidiApiMessage.createResponse({
    required MidiMappingEntry mapping,
  }) = CreateResponse;
  @FreezedUnionValue('MidiMap::update')
  const factory MidiApiMessage.update({
    required MidiMappingEntry mapping,
  }) = Update;
  @FreezedUnionValue('MidiMap::remove')
  const factory MidiApiMessage.remove({required String id}) = Remove;

  factory MidiApiMessage.fromJson(Map<String, dynamic> json) =>
      _$MidiApiMessageFromJson(json);
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

  factory MidiMapping.fromJson(Map<String, dynamic> json) {
    // Early versions of firmware do not include the mode field. Set it to a
    // default value (parameter), which behaves the same as those firmware
    // versions.
    final mode = json.containsKey('mode')
        ? MidiMappingMode.values.firstWhere((v) => v.apiName == json['mode'])
        : MidiMappingMode.parameter;

    return MidiMapping(
      midiChannel: json['midi_channel'] as int,
      ccNumber: json['cc_number'] as int,
      parameterId: json['parameter_id'] as String,
      mode: mode,
    );
  }

  Map<String, dynamic> toJson() => <String, dynamic>{
    'midi_channel': midiChannel,
    'cc_number': ccNumber,
    'parameter_id': parameterId,
    'mode': mode.apiName,
  };
}

@freezed
class MidiMappingEntry with _$MidiMappingEntry {
  const factory MidiMappingEntry({
    required String id,
    required MidiMapping mapping,
  }) = _MidiMappingEntry;

  const MidiMappingEntry._();

  factory MidiMappingEntry.fromJson(Map<String, dynamic> json) {
    if (json.length != 1) {
      throw FormatException(
        'Expected a single entry in map, but got ${json.length}',
        json,
      );
    }

    return MidiMappingEntry(
      id: json.keys.first,
      mapping: MidiMapping.fromJson(json.values.first as Map<String, dynamic>),
    );
  }

  Map<String, dynamic> toJson() {
    return <String, dynamic>{
      id: mapping.toJson(),
    };
  }
}

enum MidiMappingMode {
 toggle(apiName: 'toggle', uiName: 'Toggle'),
 parameter(apiName: 'parameter', uiName: 'Knob');

 const MidiMappingMode({required this.apiName, required this.uiName});

 final String apiName;
 final String uiName;
}
