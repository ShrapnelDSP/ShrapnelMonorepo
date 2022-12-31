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

final log = Logger('shrapnel.robust_websocket');

/// Auto-reconnecting websocket client
class RobustWebsocket extends ChangeNotifier {
  RobustWebsocket({required this.uri}) {
    unawaited(_connect());
  }

  Future<void> _reconnect() async {
    log.warning('websocket close code ${_ws!.closeCode}');
    log.warning('websocket close reason ${_ws!.closeReason}');

    isAlive = false;
    notifyListeners();

    await Future<void>.delayed(const Duration(seconds: 5));

    await _connect();
  }

  Uri uri;
  bool isAlive = false;
  void Function()? onConnect;

  WebSocket? _ws;

  Future<void> _connect() async {
    log.info('Connecting to server...');

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
        log.warning('Failed to connect to server, retrying...');
        log.warning('$err');
        await Future<void>.delayed(const Duration(seconds: 1));
      }
    }

    log.info('Connected to server');

    isAlive = true;
    notifyListeners();

    _ws = WebSocket.fromUpgradedSocket(socket, serverSide: false);

    _ws!.listen(
      _streamController.add,
      onError: (Object e) {
        log.warning('websocket error: $e');
      },
      onDone: _reconnect,
    );

    _ws!.pingInterval = const Duration(seconds: 1);

    onConnect?.call();
  }

  final _streamController = StreamController<dynamic>.broadcast();
  Stream<dynamic> get stream => _streamController.stream;

  void sendMessage(String s) {
    _ws?.add(s);
  }

  @override
  void dispose() {
    unawaited(_ws?.close(1001));
    unawaited(_streamController.close());
    super.dispose();
  }
}
