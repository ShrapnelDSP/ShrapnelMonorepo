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
import 'dart:convert';
import 'dart:io';
import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';

import 'core/message_transport.dart';

part 'robust_websocket.freezed.dart';

final _log = Logger('shrapnel.robust_websocket');

/// Auto-reconnecting websocket client
class RobustWebsocket extends ChangeNotifier
    implements ReconnectingMessageTransport<WebSocketData, WebSocketData> {
  RobustWebsocket({required this.uri}) {
    unawaited(_connect());
    _sinkController.stream.listen(_handleSinkEvent);
  }

  bool _disposed = false;

  Uri uri;

  @override
  bool isAlive = false;

  final _connectionController = StreamController<void>.broadcast();
  final _streamController = StreamController<WebSocketData>.broadcast();
  final _sinkController = StreamController<WebSocketData>();

  @override
  Stream<void> get connectionStream => _connectionController.stream;

  @override
  StreamSink<WebSocketData> get sink => _sinkController;

  @override
  Stream<WebSocketData> get stream => _streamController.stream;

  WebSocketTransport? _transport;

  Future<void> _reconnect() async {
    if (_disposed) {
      return;
    }

    _log.warning('websocket close code ${_transport!.websocket.closeCode}');
    _log.warning('websocket close reason ${_transport!.websocket.closeReason}');

    isAlive = false;
    notifyListeners();

    await Future<void>.delayed(const Duration(seconds: 5));

    if (_disposed) {
      return;
    }

    await _connect();
  }

  Future<void> _connect() async {
    _log.info('Connecting to server...');

    // We are passing the sink over to the WebSocket client, and closing it in
    // dispose. It might be incorrect if we call dispose before the socket is
    // passed to the WebSocket client?
    // ignore: close_sinks
    Socket? socket;
    while (socket == null) {
      try {
        final client = HttpClient();
        client.connectionTimeout = const Duration(seconds: 15);
        final request = await client.openUrl('GET', uri);

        final nonce = <int>[];
        final rand = Random();

        for (var i = 0; i < 16; i++) {
          nonce.add(rand.nextInt(255));
        }

        request.headers
          ..set('Connection', 'Upgrade')
          ..set(
            'Upgrade',
            'websocket',
          )
          ..set(
            'Sec-WebSocket-Key',
            base64.encode(nonce),
          )
          ..set(
            'Sec-WebSocket-Version',
            '13',
          );

        final response = await request.close();
        socket = await response.detachSocket();
      } catch (err) {
        _log.warning('Failed to connect to server, retrying...');
        _log.warning('$err');
        await Future<void>.delayed(const Duration(seconds: 1));
      }
    }

    _log.info('Connected to server');

    // The WebSocketTransport takes ownership of this sink
    // ignore: close_sinks
    final websocket = WebSocket.fromUpgradedSocket(socket, serverSide: false)
      ..pingInterval = const Duration(seconds: 1);

    final transport =
        WebSocketTransport(websocket: websocket, onDone: _reconnect);
    transport.stream.listen(_streamController.add);

    _transport = transport;
    isAlive = true;
    _connectionController.add(null);
    notifyListeners();
  }

  @override
  void dispose() {
    _disposed = true;
    super.dispose();
    unawaited(_streamController.close());
    unawaited(_sinkController.close());
    unawaited(_connectionController.close());
    _transport?.dispose();
  }

  void _handleSinkEvent(WebSocketData event) {
    _transport?.sink.add(event);
  }
}

@freezed
class WebSocketData with _$WebSocketData {
  factory WebSocketData.text({required String value}) = WebSocketDataText;

  factory WebSocketData.binary({required List<int> value}) =
      WebSocketDataBinary;
}

class WebSocketTransport
    implements MessageTransport<WebSocketData, WebSocketData> {
  WebSocketTransport({
    required this.websocket,
    required void Function() onDone,
  }) {
    websocket.listen(
      (event) => _streamController.add(
        switch (event) {
          final String text => WebSocketData.text(value: text),
          final List<int> binary => WebSocketData.binary(value: binary),
          _ => throw StateError('unexpected data type'),
        },
      ),
      onError: (Object e) {
        _log.warning('websocket error: $e');
      },
      onDone: onDone,
    );

    unawaited(
      websocket.addStream(_sinkController.stream.map((event) => event.value)),
    );
  }

  WebSocket websocket;

  final _streamController = StreamController<WebSocketData>.broadcast();

  final _sinkController = StreamController<WebSocketData>.broadcast();

  @override
  void dispose() {
    unawaited(websocket.close(1001));
    unawaited(_streamController.close());
    unawaited(_sinkController.close());
  }

  @override
  StreamSink<WebSocketData> get sink => _sinkController;

  @override
  Stream<WebSocketData> get stream => _streamController.stream;
}
