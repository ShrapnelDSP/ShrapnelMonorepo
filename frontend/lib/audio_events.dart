import 'package:flutter/material.dart';

import 'json_websocket.dart';

class AudioClippingService extends ChangeNotifier {
  AudioClippingService({required JsonWebsocket websocket}) {
    websocket.stream.listen(_handleMessage);
  }

  bool inputIsClipped = false;
  bool outputIsClipped = false;

  void _handleMessage(Map<String, dynamic> message) {
    if (message['messageType'] == 'Event::input_clipped') {
      inputIsClipped = true;
      notifyListeners();
      Future<void>.delayed(const Duration(milliseconds: 1100)).then((_) {
        inputIsClipped = false;
        notifyListeners();
      });
    } else if (message['messageType'] == 'Event::output_clipped') {
      outputIsClipped = true;
      notifyListeners();
      Future<void>.delayed(const Duration(milliseconds: 1100)).then((_) {
        outputIsClipped = false;
        notifyListeners();
      });
    }
  }
}
