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

import 'dart:convert';

import 'robust_websocket.dart';

class JsonWebsocket {
  JsonWebsocket({
    required RobustWebsocket websocket,
  }) : _websocket = websocket;

  final RobustWebsocket _websocket;

  /// The stream of incoming messages
  Stream<Map<String, dynamic>> get dataStream => _websocket.dataStream.map(
        (dynamic event) => json.decode(event as String) as Map<String, dynamic>,
      );

  /// A null is emitted every time a connection is successfully created
  Stream<void> get connectionStream => _websocket.connectionStream;

  bool get isAlive => _websocket.isAlive;

  void send(Map<String, dynamic> json) {
    _websocket.sendMessage(jsonEncode(json));
  }
}
