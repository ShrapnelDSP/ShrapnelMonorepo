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

import 'dart:async';

import 'package:async/async.dart';
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';
import 'package:rxdart/rxdart.dart';

import '../audio_events.dart';
import '../core/message_transport.dart';
import '../core/stream_extensions.dart';
import '../midi_mapping/model/models.dart';
import '../parameter.dart';
import '../presets/model/presets_client.dart';
import '../presets/model/selected_preset_client.dart';
import '../robust_websocket.dart';
import 'generated/shrapnel.pb.dart' as shrapnel_pb;
import 'proto_extension.dart';

part 'api_websocket.freezed.dart';

part 'api_websocket.g.dart';

final _log = Logger('api_websocket');

@Riverpod(keepAlive: true)
String normalHost(NormalHostRef ref) => 'guitar-dsp.local';

@Riverpod(keepAlive: true)
String provisioningHost(ProvisioningHostRef ref) => 'guitar-dsp.local';

@visibleForTesting
final kShrapnelUri = Uri(
  scheme: 'http',
  host: 'guitar-dsp.local',
  port: 8080,
  path: '/websocket',
);

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

@riverpod
Uri shrapnelUri(ShrapnelUriRef ref) {
  return kShrapnelUri.replace(host: ref.watch(normalHostProvider));
}

@riverpod
ApiWebsocket? apiWebsocket(ApiWebsocketRef ref) {
  final websocket =
      ref.watch(robustWebsocketProvider(ref.watch(shrapnelUriProvider)));
  if (websocket.isAlive) {
    return ApiWebsocket(websocket: websocket);
  }

  return null;
}

class ApiWebsocket implements MessageTransport<ApiMessage, ApiMessage> {
  ApiWebsocket({
    required MessageTransport<WebSocketData, WebSocketData> websocket,
  }) : _websocket = websocket;

  final MessageTransport<WebSocketData, WebSocketData> _websocket;

  @override
  late final Stream<ApiMessage> stream = _websocket.stream
      .whereType<WebSocketDataBinary>()
      .map((event) => event.value)
      .map(shrapnel_pb.Message.fromBuffer)
      .map(ApiMessageProtoEx.fromProto)
      .logFinest(
        _log,
        (event) => 'received: $event',
      );

  @override
  late final StreamSink<ApiMessage> sink =
      StreamSinkTransformer<ApiMessage, WebSocketData>.fromHandlers(
    handleData: (message, sink) {
      _log.finest('sending: $message');
      sink.add(WebSocketData.binary(value: message.toProto().writeToBuffer()));
    },
  ).bind(_websocket.sink);

  @override
  void dispose() {
    _websocket.dispose();
    unawaited(sink.close());
  }
}
