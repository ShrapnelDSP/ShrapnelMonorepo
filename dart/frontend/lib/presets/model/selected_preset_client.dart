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

import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';
import 'package:shrapnel_common/api_data/api_message.dart';
import 'package:shrapnel_common/api_data/selected_preset_data.dart';

import '../../api/api_websocket.dart';
import '../../core/message_transport.dart';
import '../../core/stream_extensions.dart';

final _log = Logger('selected_preset_client');

class SelectedPresetTransport
    implements
        ReconnectingMessageTransport<SelectedPresetMessage,
            SelectedPresetMessage> {
  SelectedPresetTransport({required ApiWebsocket websocket})
      : _websocket = websocket {
    _controller.stream
        .logFinest(_log, (event) => 'send message: $event')
        .map((message) => ApiMessage.selectedPreset(message: message))
        .listen(_websocket.sink.add);
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

class SelectedPresetClient {
  SelectedPresetClient({
    required ReconnectingMessageTransport<SelectedPresetMessage,
            SelectedPresetMessage>
        transport,
  }) : _transport = transport;

  final ReconnectingMessageTransport<SelectedPresetMessage,
      SelectedPresetMessage> _transport;

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
