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

import '../../api/api_websocket.dart';
import '../../core/message_transport.dart';
import '../../core/stream_extensions.dart';

part 'selected_preset_client.freezed.dart';

part 'selected_preset_client.g.dart';

final _log = Logger('selected_preset_client');

@freezed
sealed class SelectedPresetMessage with _$SelectedPresetMessage {
  factory SelectedPresetMessage.read() = ReadSelectedPresetMessage;

  factory SelectedPresetMessage.notify({required int selectedPreset}) =
      NotifySelectedPresetMessage;

  factory SelectedPresetMessage.write({required int selectedPreset}) =
      WriteSelectedPresetMessage;
}

@riverpod
SelectedPresetTransport selectedPresetTransport(
  SelectedPresetTransportRef ref,
) =>
    SelectedPresetTransport(websocket: ref.read(apiWebsocketProvider));

class SelectedPresetTransport
    implements MessageTransport<SelectedPresetMessage, SelectedPresetMessage> {
  SelectedPresetTransport({required ApiWebsocket websocket})
      : _websocket = websocket {
    _controller.stream
        .logFinest(_log, (event) => 'send message: $event')
        .listen(
          (message) =>
              _websocket.send(ApiMessage.selectedPreset(message: message)),
        );
  }

  final _controller = StreamController<SelectedPresetMessage>();

  @override
  StreamSink<SelectedPresetMessage> get sink => _controller;

  @override
  late final stream = _websocket.stream
      .whereType<ApiMessageSelectedPreset>()
      .map((event) => event.message)
      .logFinest(_log, (event) => 'received message: $event');

  @override
  Stream<void> get connectionStream => _websocket.connectionStream;

  final ApiWebsocket _websocket;

  @override
  void dispose() {
    unawaited(_controller.close());
  }

  @override
  bool get isAlive => _websocket.isAlive;
}

@riverpod
SelectedPresetClient selectedPresetClient(SelectedPresetClientRef ref) =>
    SelectedPresetClient(transport: ref.read(selectedPresetTransportProvider));

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
