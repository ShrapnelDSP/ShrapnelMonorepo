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
import 'package:flutter/material.dart';
import 'package:logging/logging.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';

import 'riverpod_util.dart';

part 'robust_websocket.g.dart';

final _log = Logger('shrapnel.robust_websocket');

@riverpod
// ignore: unsupported_provider_value
RobustWebsocket robustWebsocket(RobustWebsocketRef ref, Uri uri) =>
    ref.listenAndDisposeChangeNotifier(RobustWebsocket(uri: uri));

/// Auto-reconnecting websocket client
class RobustWebsocket extends ChangeNotifier {
  RobustWebsocket({required this.uri}) {
    unawaited(_connect());
  }

  Future<void> _reconnect() async {
    _log.warning('websocket close code ${_ws!.closeCode}');
    _log.warning('websocket close reason ${_ws!.closeReason}');

    isAlive = false;
    notifyListeners();

    await Future<void>.delayed(const Duration(seconds: 5));

    await _connect();
  }

  Uri uri;
  bool isAlive = false;

  final _connectionController = StreamController<void>.broadcast();

  /// A null is emitted every time a connection is successfully created
  Stream<void> get connectionStream => _connectionController.stream;

  final _dataController = StreamController<dynamic>.broadcast();

  /// Equivalent to the stream of a [WebSocket]
  Stream<dynamic> get dataStream => _dataController.stream;

  WebSocket? _ws;

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
          ..set('Upgrade', 'websocket')
          ..set('Sec-WebSocket-Key', base64.encode(nonce))
          ..set('Sec-WebSocket-Version', '13');

        final response = await request.close();
        socket = await response.detachSocket();
      } catch (err) {
        _log.warning('Failed to connect to server, retrying...');
        _log.warning('$err');
        await Future<void>.delayed(const Duration(seconds: 1));
      }
    }

    _log.info('Connected to server');

    isAlive = true;
    notifyListeners();

    _ws = WebSocket.fromUpgradedSocket(socket, serverSide: false);

    _ws!.listen(
      _dataController.add,
      onError: (Object e) {
        _log.warning('websocket error: $e');
      },
      onDone: _reconnect,
    );

    _ws!.pingInterval = const Duration(seconds: 1);

    _connectionController.add(null);
  }

  void sendMessage(List<int> s) {
    _ws?.add(s);
  }

  @override
  void dispose() {
    unawaited(_connectionController.close());
    unawaited(_ws?.close(1001));
    unawaited(_dataController.close());
    super.dispose();
  }
}
