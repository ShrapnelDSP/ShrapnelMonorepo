import 'dart:async';
import 'dart:collection';

import 'package:flutter/material.dart';
import 'package:tuple/tuple.dart';

import '../../json_websocket.dart';
import '../../util/tuple_extensions.dart';
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
  /// TODO:
  /// This is updated optimistically with respect to the back end. When create
  /// is called, the mapping is added, assuming that the backend will not error.
  /// If there is an error, the mapping is removed.
  UnmodifiableMapView<String, MidiMapping> get mappings => _mappingsView;

  JsonWebsocket websocket;

  void getMapping() {
    const message = MidiApiMessage.getRequest();

    websocket.stream
        .map(
          MidiApiMessage.fromJson,
        )
        .where((event) => event is GetResponse)
        .map((event) => event as GetResponse)
        .map((event) => event.mappings)
        .timeout(
          responseTimeout,
          onTimeout: (_) => throw TimeoutException(
              'Waiting for response to get request timed out',),
        )
        .first
        .then((value) {
          __mappings.clear();
          __mappings.addAll(value);
          notifyListeners();
        });

    websocket.send(message.toJson());
  }

  void createMapping(
    Tuple2<String, MidiMapping> mapping,
  ) {
    final response = websocket.stream
        .map(
          MidiApiMessage.fromJson,
        )
        .where((event) => event is CreateResponse)
        .map((event) => event as CreateResponse)
        .map(
          (event) => event.mapping.toTuple2(),
        )
        .timeout(
          responseTimeout,
          onTimeout: (_) => throw TimeoutException(
              'Waiting for response to create request timed out',),
        )
        .firstWhere((event) => event == mapping);

    // optimistically add the mapping to the UI state
    __mappings.addAll(mapping.toMap());
    notifyListeners();

    // roll back if there was an error
    /*
    unawaited(
      response.onError(
        (e, __) {
          __mappings.remove(mapping.item1);
          notifyListeners();
          throw e! as Exception;
        },
        test: (error) => error is TimeoutException,
      ),
    );
    */

    final message = MidiApiMessage.createRequest(mapping: mapping.toMap());
    websocket.send(message.toJson());
  }

  Future<void> updateMapping(Tuple2<String, MidiMapping> mapping) async {
    __mappings.addAll(mapping.toMap());
    notifyListeners();

    final message = MidiApiMessage.update(mapping: mapping.toMap());
    websocket.send(message.toJson());
  }

  Future<void> deleteMapping({required String id}) async {
    __mappings.remove(id);
    notifyListeners();

    final message = MidiApiMessage.remove(id: id);
    websocket.send(message.toJson());
  }
}
