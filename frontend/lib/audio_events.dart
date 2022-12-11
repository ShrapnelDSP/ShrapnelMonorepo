import 'package:flutter/material.dart';
import 'package:logging/logging.dart';

import 'json_websocket.dart';

final _log = Logger('audio_events');

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
      _log.finer('clipped');
      notifyListeners();
      // TODO do we need to cancel any previous timers? The light is not getting lit for 1s currently
      // flutter:   FINER 19:20:13.943 audio_events: clipped
      // flutter:   FINER 19:20:13.943 parameter: {"messageType":"Event::output_clipped"}
      // flutter:   FINER 19:20:14.032 audio_events: not clipped
      // Because of the timing difference, the previous event's timer is
      // cancelling the colour. We need a single timer that is restarted on
      // repeat clipping events. The clipping flag should go off only when the
      // timer expires.
      Future<void>.delayed(const Duration(milliseconds: 1100)).then((_) {
        outputIsClipped = false;
        _log.finer('not clipped');
        notifyListeners();
      });
    }
  }
}
