import 'dart:async';
import 'dart:convert';

import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';

import '../../core/message_transport.dart';
import '../../core/stream_extensions.dart';
import '../../json_websocket.dart';
import '../proto/generated/presets.pb.dart' as pb;

import 'presets.dart' as presets;

part 'presets_client.freezed.dart';

part 'presets_client.g.dart';

// ignore: unused_element
final _log = Logger('presets_client');

class _PresetParametersJsonConverter
    implements JsonConverter<PresetParametersData, String> {
  const _PresetParametersJsonConverter();

  @override
  PresetParametersData fromJson(String json) {
    final parameters = pb.PresetParameters.fromBuffer(base64Decode(json));
    return PresetParametersData(
      ampGain: parameters.ampGain / 1000,
      ampChannel: parameters.ampChannel / 1000,
      bass: parameters.bass / 1000,
      middle: parameters.middle / 1000,
      treble: parameters.treble / 1000,
      contour: parameters.contour / 1000,
      volume: parameters.volume / 1000,
      noiseGateThreshold: parameters.noiseGateThreshold / 1000,
      noiseGateHysteresis: parameters.noiseGateHysteresis / 1000,
      noiseGateAttack: parameters.noiseGateAttack / 1000,
      noiseGateHold: parameters.noiseGateHold / 1000,
      noiseGateRelease: parameters.noiseGateRelease / 1000,
      noiseGateBypass: parameters.noiseGateBypass / 1000,
      chorusRate: parameters.chorusRate / 1000,
      chorusDepth: parameters.chorusDepth / 1000,
      chorusMix: parameters.chorusMix / 1000,
      chorusBypass: parameters.chorusBypass / 1000,
      wahPosition: parameters.wahPosition / 1000,
      wahVocal: parameters.wahVocal / 1000,
      wahBypass: parameters.wahBypass / 1000,
    );
  }

  @override
  String toJson(PresetParametersData object) {
    final parameters = pb.PresetParameters(
      ampGain: (object.ampGain * 1000).round(),
      ampChannel: (object.ampChannel * 1000).round(),
      bass: (object.bass * 1000).round(),
      middle: (object.middle * 1000).round(),
      treble: (object.treble * 1000).round(),
      contour: (object.contour * 1000).round(),
      volume: (object.volume * 1000).round(),
      noiseGateThreshold: (object.noiseGateThreshold * 1000).round(),
      noiseGateHysteresis: (object.noiseGateHysteresis * 1000).round(),
      noiseGateAttack: (object.noiseGateAttack * 1000).round(),
      noiseGateHold: (object.noiseGateHold * 1000).round(),
      noiseGateRelease: (object.noiseGateRelease * 1000).round(),
      noiseGateBypass: (object.noiseGateBypass * 1000).round(),
      chorusRate: (object.chorusRate * 1000).round(),
      chorusDepth: (object.chorusDepth * 1000).round(),
      chorusMix: (object.chorusMix * 1000).round(),
      chorusBypass: (object.chorusBypass * 1000).round(),
      wahPosition: (object.wahPosition * 1000).round(),
      wahVocal: (object.wahVocal * 1000).round(),
      wahBypass: (object.wahBypass * 1000).round(),
    );
    return base64Encode(parameters.writeToBuffer());
  }
}

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

  // ignore: prefer_constructors_over_static_methods
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
  @_PresetParametersJsonConverter()
  factory PresetData({
    required String name,
    required PresetParametersData parameters,
  }) = _PresetData;

  factory PresetData.fromJson(Map<String, dynamic> json) =>
      _$PresetDataFromJson(json);

  // ignore: prefer_constructors_over_static_methods
  static PresetData fromPresetState(presets.PresetState presetState) {
    return PresetData(
      name: presetState.name,
      parameters: PresetParametersData.fromData(presetState.parameters),
    );
  }
}

@Freezed(unionKey: 'messageType')
class PresetsMessage with _$PresetsMessage {
  @FreezedUnionValue('Presets::initialise')
  factory PresetsMessage.initialise() = PresetsMessageInitialise;

  @FreezedUnionValue('Presets::notify')
  factory PresetsMessage.notify(int id, PresetData preset) =
      PresetsMessageNotify;

  @FreezedUnionValue('Presets::create')
  factory PresetsMessage.create(PresetData preset) = PresetsMessageCreate;

  @FreezedUnionValue('Presets::update')
  factory PresetsMessage.update(int id, PresetData preset) =
      PresetsMessageUpdate;

  @FreezedUnionValue('Presets::delete')
  factory PresetsMessage.delete(int id) = PresetsMessageDelete;

  factory PresetsMessage.fromJson(Map<String, dynamic> json) =>
      _$PresetsMessageFromJson(json);
}

class PresetsTransport
    implements MessageTransport<PresetsMessage, PresetsMessage> {
  PresetsTransport({required this.websocket}) {
    _controller.stream
        .logFinest(_log, (event) => 'send message: $event')
        .listen((message) => websocket.send(message.toJson()));
  }

  final _controller = StreamController<PresetsMessage>();

  @override
  StreamSink<PresetsMessage> get sink => _controller.sink;

  @override
  late final Stream<PresetsMessage> stream = websocket.dataStream.transform(
    StreamTransformer.fromBind((jsonStream) async* {
      await for (final message in jsonStream) {
        try {
          yield PresetsMessage.fromJson(message);
        } catch (_) {
          // ignore
        }
      }
    }),
  ).logFinest(
    _log,
    (event) => 'receive message: $event',
  );

  @override
  Stream<void> get connectionStream => websocket.connectionStream;

  JsonWebsocket websocket;

  @override
  void dispose() {
    unawaited(_controller.close());
  }
}

class PresetsClient {
  PresetsClient({
    required MessageTransport<PresetsMessage, PresetsMessage> transport,
  }) : _transport = transport;

  final MessageTransport<PresetsMessage, PresetsMessage> _transport;

  /// Firmware will reply by sending a preset update message for every preset
  /// that already exists.
  Future<void> initialise() async {
    _transport.sink.add(PresetsMessage.initialise());
  }

  late final Stream<presets.PresetRecord> presetUpdates = _transport.stream
      .whereType<PresetsMessageNotify>()
      .map(
        (event) => presets.PresetRecord(
          id: event.id,
          preset: presets.PresetState(
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
        ),
      );

  Future<void> create(presets.PresetState preset) async {
    _transport.sink
        .add(PresetsMessage.create(PresetData.fromPresetState(preset)));
  }

  Future<void> delete(int id) async {
    _transport.sink.add(PresetsMessage.delete(id));
  }

  Future<void> update(presets.PresetRecord preset) async {
    _transport.sink.add(
      PresetsMessage.update(
        preset.id,
        PresetData.fromPresetState(preset.preset),
      ),
    );
  }

  Stream<void> get connectionStream => _transport.connectionStream;

  void dispose() {
    _transport.dispose();
  }
}
