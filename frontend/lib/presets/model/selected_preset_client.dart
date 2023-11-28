import 'dart:async';

import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:rxdart/rxdart.dart';
import 'package:uuid/uuid.dart';

import '../../core/message_transport.dart';
import '../../core/uuid_json_converter.dart';
import '../../json_websocket.dart';

part 'selected_preset_client.freezed.dart';

part 'selected_preset_client.g.dart';

@freezed
sealed class SelectedPresetMessage with _$SelectedPresetMessage {
  factory SelectedPresetMessage.read() = ReadSelectedPresetMessage;

  @UuidJsonConverter()
  factory SelectedPresetMessage.notify({required UuidValue selectedPresetId}) =
      NotifySelectedPresetMessage;

  @UuidJsonConverter()
  factory SelectedPresetMessage.write({required UuidValue selectedPresetId}) =
      WriteSelectedPresetMessage;

  factory SelectedPresetMessage.fromJson(Map<String, dynamic> json) =>
      _$SelectedPresetMessageFromJson(json);
}

class SelectedPresetTransport
    implements MessageTransport<SelectedPresetMessage, SelectedPresetMessage> {
  SelectedPresetTransport({required this.websocket}) {
    _controller.stream.listen((message) {
      websocket.send(message.toJson());
    });
  }

  final _controller = StreamController<SelectedPresetMessage>();

  @override
  StreamSink<SelectedPresetMessage> get sink => _controller;

  @override
  Stream<SelectedPresetMessage> get stream => websocket.dataStream.transform(
        StreamTransformer.fromBind((jsonStream) async* {
          await for (final message in jsonStream) {
            yield SelectedPresetMessage.fromJson(message);
          }
        }),
      );

  JsonWebsocket websocket;

  @override
  void dispose() {
    unawaited(_controller.close());
  }
}

class SelectedPresetClient {
  SelectedPresetClient({required this.transport});

  MessageTransport<SelectedPresetMessage, SelectedPresetMessage> transport;

  void initialise() {
    transport.sink.add(SelectedPresetMessage.read());
  }

  Stream<UuidValue> get selectedPreset => transport.stream
      .whereType<NotifySelectedPresetMessage>()
      .map((event) => event.selectedPresetId);

  Future<void> selectPreset(UuidValue presetId) async {
    transport.sink.add(SelectedPresetMessage.write(selectedPresetId: presetId));
  }
}
