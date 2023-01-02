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
import 'dart:collection';

import 'package:flutter/material.dart';

import '../../json_websocket.dart';
import '../model/models.dart';

class MidiMappingService extends ChangeNotifier {
  MidiMappingService({
    required this.websocket,
  }) {
    _mappingsView = UnmodifiableMapView(__mappings);

    websocket.connectionStream.listen((_) async => getMapping());

    if (websocket.isAlive) {
      unawaited(getMapping());
    }
  }

  static const responseTimeout = Duration(milliseconds: 500);

  final __mappings = <String, MidiMapping>{};

  late UnmodifiableMapView<String, MidiMapping> _mappingsView;

  /// The mappings that are currently considered to be present.
  ///
  /// This is updated optimistically with respect to the back end. When create
  /// is called, the mapping is added, assuming that the backend will not error.
  /// If there is an error, the mapping is removed.
  UnmodifiableMapView<String, MidiMapping> get mappings => _mappingsView;

  JsonWebsocket websocket;

  Future<void> getMapping() async {
    const message = MidiApiMessage.getRequest();

    final response = websocket.dataStream
        .map(
          MidiApiMessage.fromJson,
        )
        .where((event) => event is GetResponse)
        .map((event) => event as GetResponse)
        .map((event) => event.mappings)
        .timeout(
          responseTimeout,
        )
        .first;

    websocket.send(message.toJson());

    try {
      final value = await response;
      __mappings.clear();
      __mappings.addAll(value);
      notifyListeners();
    } on TimeoutException {
      // ignore
    }
  }

  // TODO this should not take a UUID, instead it should create it internally
  Future<void> createMapping(
    MidiMappingEntry mapping,
  ) async {
    final response = websocket.dataStream
        .map(
          MidiApiMessage.fromJson,
        )
        .where((event) => event is CreateResponse)
        .map((event) => event as CreateResponse)
        .map(
          (event) => event.mapping,
        )
        .where((event) => event == mapping)
        .timeout(responseTimeout)
        .first;

    // optimistically add the mapping to the UI state
    __mappings[mapping.id] = mapping.mapping;
    notifyListeners();

    final message = MidiApiMessage.createRequest(mapping: mapping);
    websocket.send(message.toJson());

    try {
      await response;
    } on TimeoutException {
      __mappings.remove(mapping.id);
      notifyListeners();
      rethrow;
    }
  }

  Future<void> updateMapping(MidiMappingEntry mapping) async {
    __mappings[mapping.id] = mapping.mapping;
    notifyListeners();

    final message = MidiApiMessage.update(mapping: mapping);
    websocket.send(message.toJson());
  }

  Future<void> deleteMapping({required String id}) async {
    __mappings.remove(id);
    notifyListeners();

    final message = MidiApiMessage.remove(id: id);
    websocket.send(message.toJson());
  }
}
