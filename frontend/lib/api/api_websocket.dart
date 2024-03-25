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

import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';
import 'package:rxdart/rxdart.dart';

import '../audio_events.dart';
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

const kShrapnelUri = 'http://guitar-dsp.local:8080/websocket';

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
ApiWebsocket? apiWebsocket(ApiWebsocketRef ref) {
  final websocket = ref.watch(robustWebsocketProvider(Uri.parse(kShrapnelUri)));
  if (websocket.isAlive) {
    return ApiWebsocket(websocket: websocket);
  }

  return null;
}

// TODO make this implement MessageTransport<ApiMessage, ApiMessage>
class ApiWebsocket {
  ApiWebsocket({required RobustWebsocket websocket}) : _websocket = websocket;

  final RobustWebsocket _websocket;

  /// The stream of incoming messages
  late final Stream<ApiMessage> stream = _websocket.dataStream
      .whereType<List<int>>()
      .map(shrapnel_pb.Message.fromBuffer)
      .map(ApiMessageProtoEx.fromProto)
      .logFinest(
        _log,
        (event) => 'received: $event',
      );

  void send(ApiMessage message) {
    _log.finest('sending: $message');
    _websocket.sendMessage(message.toProto().writeToBuffer());
  }
}
