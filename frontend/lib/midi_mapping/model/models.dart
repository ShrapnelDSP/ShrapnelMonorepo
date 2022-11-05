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
  }) = _MidiMapping;

  factory MidiMapping.fromJson(Map<String, dynamic> json) =>
      _$MidiMappingFromJson(json);
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
