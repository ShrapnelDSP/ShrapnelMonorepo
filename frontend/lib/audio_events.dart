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

import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:pausable_timer/pausable_timer.dart';
import 'package:rxdart/rxdart.dart';

import 'api/api_websocket.dart';

part 'audio_events.freezed.dart';

final _log = Logger('shrapnel.audio_events');

@freezed
sealed class AudioEventMessage with _$AudioEventMessage {
  factory AudioEventMessage.inputClipped() = AudioEventMessageInputClipped;

  factory AudioEventMessage.outputClipped() = AudioEventMessageOutputClipped;
}

final audioClippingServiceProvider = AutoDisposeChangeNotifierProvider(
  (ref) {
    final websocket = ref.watch(apiWebsocketProvider);
    if (websocket != null) {
      return AudioClippingService(
        stream: websocket.stream
            .whereType<ApiMessageAudioEvent>()
            .map((event) => event.message),
      );
    }

    return null;
  },
);

class AudioClippingService extends ChangeNotifier {
  AudioClippingService({required Stream<AudioEventMessage> stream}) {
    _outputClippingTimer = PausableTimer(
      const Duration(milliseconds: 1100),
      _clearOutputClipping,
    );

    _inputClippingTimer = PausableTimer(
      const Duration(milliseconds: 1100),
      _clearInputClipping,
    );

    stream.listen(_handleMessage);
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

  void _handleMessage(AudioEventMessage message) {
    switch (message) {
      case AudioEventMessageInputClipped():
        inputIsClipped = true;
        notifyListeners();

        _inputClippingTimer
          ..reset()
          ..start();
      case AudioEventMessageOutputClipped():
        outputIsClipped = true;
        _log.finer('clipped');
        notifyListeners();

        _outputClippingTimer
          ..reset()
          ..start();
    }
  }
}
