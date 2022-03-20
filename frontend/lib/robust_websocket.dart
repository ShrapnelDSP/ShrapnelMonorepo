import 'dart:convert';
import 'dart:io';
import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:logging/logging.dart';

final log = Logger('robust_websocket');

class RobustWebsocket extends ChangeNotifier {
  RobustWebsocket({required this.uri}) {
    _connect();
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
  void Function(dynamic)? onData;

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
        client.connectionTimeout = const Duration(seconds: 5);
        final request = await client.openUrl('GET', uri);

        final nonce = <int>[];
        final rand = Random();

        for (var i = 0; i < 16; i++)
        {
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

    _ws!.listen((dynamic data) {
      onData?.call(data);
    }, onError: (Object e) {
      log.warning('websocket error: $e');
    }, onDone: _reconnect);

    _ws!.pingInterval = const Duration(seconds: 1);
  }

  void sendMessage(String s) {
    _ws?.add(s);
  }

  @override
  void dispose() {
    _ws?.close(1001);
    super.dispose();
  }
}
