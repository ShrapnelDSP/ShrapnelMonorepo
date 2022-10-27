import 'dart:convert';

import 'robust_websocket.dart';

class JsonWebsocket {
  JsonWebsocket({
    required this.websocket,
  });

  final RobustWebsocket websocket;

  Stream<Map<String, dynamic>> get stream => websocket.stream.map(
        (dynamic event) => json.decode(event as String) as Map<String, dynamic>,
      );

  void send(Map<String, dynamic> json) {
    websocket.sendMessage(jsonEncode(json));
  }
}
