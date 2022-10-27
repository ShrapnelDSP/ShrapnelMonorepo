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
    const message = <String, dynamic>{
      'messageType': 'MidiMap::get::request',
    };

    final response = websocket.stream
        .where(
          (event) => event.isGetResponse(),
        )
        .timeout(
          responseTimeout,
          onTimeout: (_) =>
              throw TimeoutException('Waiting for response timed out'),
        )
        .map(
          (event) => GetResponse.fromJson(event).mappings,
        )
        .first;
    websocket.send(message);
    unawaited(response.then((value) => mappings = value));
    return response;
  }

  Future<Tuple2<String, MidiMapping>> createMapping(MidiMapping mapping) async {
    assert(createMappingCompleter == null);

    createMappingCompleter = Completer();
    return createMappingCompleter!.future;
  }
}

extension on Map<String, dynamic> {
  bool isGetResponse() =>
      containsKey('messageType') &&
      this['messageType'] == 'MidiMap::get::response';
}
