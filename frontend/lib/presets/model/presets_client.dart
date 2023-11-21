import 'dart:async';

import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:uuid/uuid.dart';

import '../../json_websocket.dart';

import 'presets.dart' as presets;

part 'presets_client.freezed.dart';

final _log = Logger('presets_client');

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

  // FIXME: this probably needs to be encoded to protobuf to save size
  // Maybe add it to the JSON as base64 encoded binary that can be decoded to
  // protobuf.
  factory PresetParametersData.fromJson(Map<String, dynamic> json) =>
      _$PresetParametersDataFromJson(json);

  static PresetParametersData fromData(
    presets.PresetParametersData parameters,
  ) {
    return PresetParametersData(
      ampChannel: parameters.ampChannel,
      ampGain: parameters.ampGain,
      bass: parameters.bass,
      chorusBypass: parameters.chorusBypass,
      chorusDepth: parameters.chorusDepth,
      chorusMix: parameters.chorusMix,
      chorusRate: parameters.chorusRate,
      contour: parameters.contour,
      middle: parameters.middle,
      noiseGateAttack: parameters.noiseGateAttack,
      noiseGateBypass: parameters.noiseGateBypass,
      noiseGateHold: parameters.noiseGateHold,
      noiseGateHysteresis: parameters.noiseGateHysteresis,
      noiseGateRelease: parameters.noiseGateRelease,
      noiseGateThreshold: parameters.noiseGateThreshold,
      treble: parameters.treble,
      volume: parameters.volume,
      wahBypass: parameters.wahBypass,
      wahPosition: parameters.wahPosition,
      wahVocal: parameters.wahVocal,
    );
  }
}

@freezed
class PresetData with _$PresetData {
  factory PresetData({
    required String id,
    required String name,
    required PresetParametersData parameters,
  }) = _PresetData;

  factory PresetData.fromJson(Map<String, dynamic> json) =>
      _$PresetDataFromJson(json);

  static PresetData fromPresetState(presets.PresetState presetState) {
    return PresetData(
        id: presetState.id.uuid,
        name: presetState.name,
        parameters: PresetParametersData.fromData(presetState.parameters));
  }
}

@freezed
class PresetsMessage with _$PresetsMessage {
  factory PresetsMessage.initialise() = PresetsMessageInitialise;

  // FIXME: also notify about presets being deleted?
  factory PresetsMessage.notify(PresetData preset) = PresetsMessageNotify;

  factory PresetsMessage.create(PresetData preset) = PresetsMessageCreate;

  factory PresetsMessage.update(PresetData preset) = PresetsMessageUpdate;

  factory PresetsMessage.delete(String id) = PresetsMessageDelete;

  factory PresetsMessage.fromJson(Map<String, dynamic> json) =>
      _$PresetsMessageFromJson(json);
}

class PresetsTransport {
  PresetsTransport({required this.websocket}) {
    _controller.stream.listen((message) {
      websocket.send(message.toJson());
    });
  }

  final _controller = StreamController<PresetsMessage>();

  StreamSink<PresetsMessage> get sink => _controller.sink;

  Stream<PresetsMessage> get stream => websocket.dataStream.transform(
        StreamTransformer.fromBind((jsonStream) async* {
          await for (final message in jsonStream) {
            yield PresetsMessage.fromJson(message);
          }
        }),
      );

  JsonWebsocket websocket;

  void dispose() {
    unawaited(_controller.close());
  }
}

class PresetsClient {
  PresetsClient({required this.transport});

  PresetsTransport transport;

  /// Firmware will reply by sending a preset update message for every preset
  /// that already exists.
  Future<void> initialise() async {
    transport.sink.add(PresetsMessage.initialise());
  }

  Stream<presets.PresetState> get presetUpdates => transport.stream
      .where((message) => message is PresetsMessageNotify)
      .cast<PresetsMessageNotify>()
      .map(
        (event) => presets.PresetState(
          id: UuidValue(event.preset.id),
          name: event.preset.name,
          parameters: presets.PresetParametersData(
            ampChannel: event.preset.parameters.ampChannel,
            ampGain: event.preset.parameters.ampGain,
            bass: event.preset.parameters.bass,
            chorusBypass: event.preset.parameters.chorusBypass,
            chorusDepth: event.preset.parameters.chorusDepth,
            chorusMix: event.preset.parameters.chorusMix,
            chorusRate: event.preset.parameters.chorusRate,
            contour: event.preset.parameters.contour,
            middle: event.preset.parameters.middle,
            noiseGateAttack: event.preset.parameters.noiseGateAttack,
            noiseGateBypass: event.preset.parameters.noiseGateBypass,
            noiseGateHold: event.preset.parameters.noiseGateHold,
            noiseGateHysteresis: event.preset.parameters.noiseGateHysteresis,
            noiseGateRelease: event.preset.parameters.noiseGateRelease,
            noiseGateThreshold: event.preset.parameters.noiseGateThreshold,
            treble: event.preset.parameters.treble,
            volume: event.preset.parameters.volume,
            wahBypass: event.preset.parameters.wahBypass,
            wahPosition: event.preset.parameters.wahPosition,
            wahVocal: event.preset.parameters.wahVocal,
          ),
        ),
      );

  Future<void> create(presets.PresetState preset) async {
    transport.sink
        .add(PresetsMessage.create(PresetData.fromPresetState(preset)));
  }

  Future<void> delete(UuidValue id) async {
    transport.sink.add(PresetsMessage.delete(id.uuid));
  }

  Future<void> update(presets.PresetState preset) async {
    transport.sink
        .add(PresetsMessage.update(PresetData.fromPresetState(preset)));
  }

  void dispose() {
    transport.dispose();
  }
}
