import 'package:flutter/material.dart';
import 'package:logging/logging.dart';
import 'package:pausable_timer/pausable_timer.dart';

import 'json_websocket.dart';

final _log = Logger('shrapnel.audio_events');

class AudioClippingService extends ChangeNotifier {
  AudioClippingService({required JsonWebsocket websocket}) {
    _outputClippingTimer = PausableTimer(
      const Duration(milliseconds: 1100),
      _clearOutputClipping,
    );

    _inputClippingTimer = PausableTimer(
      const Duration(milliseconds: 1100),
      _clearInputClipping,
    );

    websocket.stream.listen(_handleMessage);
  }

  bool inputIsClipped = false;
  bool outputIsClipped = false;

  late PausableTimer _inputClippingTimer;
  late PausableTimer _outputClippingTimer;

  void _clearInputClipping() {
    inputIsClipped = false;
    notifyListeners();
  }

  void _clearOutputClipping() {
    outputIsClipped = false;
    _log.finer('not clipped');
    notifyListeners();
  }

  void _handleMessage(Map<String, dynamic> message) {
    if (message['messageType'] == 'Event::input_clipped') {
      inputIsClipped = true;
      notifyListeners();

      _inputClippingTimer
        ..reset()
        ..start();
    } else if (message['messageType'] == 'Event::output_clipped') {
      outputIsClipped = true;
      _log.finer('clipped');
      notifyListeners();

      _outputClippingTimer
        ..reset()
        ..start();
    }
  }
}
