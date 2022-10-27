import 'dart:async';

import 'package:flutter/material.dart';
import 'package:tuple/tuple.dart';

import '../../json_websocket.dart';
import '../model/models.dart';

class MidiMappingService extends ChangeNotifier {
  MidiMappingService({
    required this.websocket,
  });

  static const responseTimeout = Duration(milliseconds: 500);

  Map<String, FutureOr<MidiMapping>> mappings = {};

  JsonWebsocket websocket;
  Completer<Tuple2<String, MidiMapping>>? createMappingCompleter;

  Future<Map<String, MidiMapping>> getMapping() async {
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
          onTimeout: (_) =>
              throw TimeoutException('Waiting for response timed out'),
        )
        .first;
    websocket.send(message.toJson());
    unawaited(response.then((value) => mappings = value));
    return response;
  }

  Future<Tuple2<String, MidiMapping>> createMapping(MidiMapping mapping) async {
    assert(createMappingCompleter == null);

    createMappingCompleter = Completer();
    return createMappingCompleter!.future;
  }
}
