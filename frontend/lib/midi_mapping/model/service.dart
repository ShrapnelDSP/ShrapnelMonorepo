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

    final response = websocket.stream
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

  Future<void> createMapping(
    MidiMappingEntry mapping,
  ) async {
    final response = websocket.stream
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
