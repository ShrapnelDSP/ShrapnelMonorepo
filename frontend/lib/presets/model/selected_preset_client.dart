import 'dart:async';

import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';

import '../../core/message_transport.dart';
import '../../core/stream_extensions.dart';
import '../../core/uuid_json_converter.dart';
import '../../json_websocket.dart';

part 'selected_preset_client.freezed.dart';

part 'selected_preset_client.g.dart';

final _log = Logger('selected_preset_client');

@Freezed(unionKey: 'messageType')
sealed class SelectedPresetMessage with _$SelectedPresetMessage {
  @FreezedUnionValue('SelectedPreset::read')
  factory SelectedPresetMessage.read() = ReadSelectedPresetMessage;

  @UuidJsonConverter()
  @FreezedUnionValue('SelectedPreset::notify')
  factory SelectedPresetMessage.notify({required int selectedPreset}) =
      NotifySelectedPresetMessage;

  @UuidJsonConverter()
  @FreezedUnionValue('SelectedPreset::write')
  factory SelectedPresetMessage.write({required int selectedPreset}) =
      WriteSelectedPresetMessage;

  factory SelectedPresetMessage.fromJson(Map<String, dynamic> json) =>
      _$SelectedPresetMessageFromJson(json);
}

class SelectedPresetTransport
    implements MessageTransport<SelectedPresetMessage, SelectedPresetMessage> {
  SelectedPresetTransport({required JsonWebsocket websocket}) : _websocket = websocket {
    _controller.stream
        .logFinest(_log, (event) => 'send message: $event')
        .listen((message) => _websocket.send(message.toJson()));
  }

  final _controller = StreamController<SelectedPresetMessage>();

  @override
  StreamSink<SelectedPresetMessage> get sink => _controller;

  @override
  late final stream = _websocket.dataStream.transform(
        StreamTransformer.fromBind((jsonStream) async* {
          await for (final message in jsonStream) {
            try {
              yield SelectedPresetMessage.fromJson(message);
            } catch (_) {
              // ignore
            }
          }
        }),
      ).logFinest(_log, (event) => 'received message: $event');

  @override
  Stream<void> get connectionStream => _websocket.connectionStream;

  final JsonWebsocket _websocket;

  @override
  void dispose() {
    unawaited(_controller.close());
  }
}

class SelectedPresetClient {
  SelectedPresetClient({
    required MessageTransport<SelectedPresetMessage, SelectedPresetMessage>
        transport,
  }) : _transport = transport;

  final MessageTransport<SelectedPresetMessage, SelectedPresetMessage>
      _transport;

  Future<void> initialise() async {
    _transport.sink.add(SelectedPresetMessage.read());
  }

  Stream<int> get selectedPreset => _transport.stream
      .whereType<NotifySelectedPresetMessage>()
      .map((event) => event.selectedPreset);

  Future<void> selectPreset(int presetId) async {
    _transport.sink.add(SelectedPresetMessage.write(selectedPreset: presetId));
  }

  Stream<void> get connectionStream => _transport.connectionStream;
}
