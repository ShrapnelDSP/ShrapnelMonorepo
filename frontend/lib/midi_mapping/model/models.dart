// ignore_for_file: invalid_annotation_target
// freezed and JsonKey generate this warning when used together correctly

import 'package:freezed_annotation/freezed_annotation.dart';

part 'models.freezed.dart';
part 'models.g.dart';


// TODO: should we have a single type that includes all the possible messages
//       that will arrive through the websocket interface? It might make parsing
//       easier, since we can just call a single fromJson for any received
//       message.

@freezed
class GetResponse with _$GetResponse {
  const factory GetResponse({
    required Map<String, MidiMapping> mappings,
  }) = _GetResponse;

  factory GetResponse.fromJson(Map<String, dynamic> json) =>
      _$GetResponseFromJson(json);
}

@freezed
class MidiMapping with _$MidiMapping {
  const factory MidiMapping({
    @JsonKey(name: 'midi_channel') required int midiChannel,
    @JsonKey(name: 'cc_number') required int ccNumber,
    @JsonKey(name: 'parameter_id') required String parameterId,
  }) = _MidiMapping;

  factory MidiMapping.fromJson(Map<String, dynamic> json) => _$MidiMappingFromJson(json);
}
