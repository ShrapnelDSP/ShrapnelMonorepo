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
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';

import '../../api/api_websocket.dart';
import '../../core/message_transport.dart';
import '../../core/stream_extensions.dart';
import '../model/models.dart';

final _log = Logger('midi_mapping_service');

final midiMappingTransportProvider = Provider(
  (ref) => MidiMappingTransport(
    websocket: ref.read(apiWebsocketProvider),
  ),
);

class MidiMappingTransport
    implements MessageTransport<MidiApiMessage, MidiApiMessage> {
  MidiMappingTransport({required this.websocket}) {
    _controller.stream
        .logFinest(_log, (event) => 'send message: $event')
        .listen(
          (message) => websocket.send(ApiMessage.midiMapping(message: message)),
        );
  }

  final _controller = StreamController<MidiApiMessage>();

  @override
  StreamSink<MidiApiMessage> get sink => _controller.sink;

  @override
  late final Stream<MidiApiMessage> stream = websocket.stream
      .whereType<ApiMessageMidiMapping>()
      .map((event) => event.message)
      .logFinest(
        _log,
        (event) => 'receive message: $event',
      );

  @override
  Stream<void> get connectionStream => websocket.connectionStream;

  ApiWebsocket websocket;

  @override
  void dispose() {
    unawaited(_controller.close());
  }

  @override
  bool get isAlive => websocket.isAlive;
}

class MidiMappingService extends ChangeNotifier {
  MidiMappingService({
    required this.websocket,
  }) {
    _mappingsView = UnmodifiableMapView(__mappings);

    websocket.connectionStream.listen((_) async => getMapping());
    _subscription = websocket.stream.listen(_handleMessage);

    if (websocket.isAlive) {
      unawaited(getMapping());
    }
  }

  static const responseTimeout = Duration(milliseconds: 500);

  final __mappings = <MidiMappingId, MidiMapping>{};

  late UnmodifiableMapView<MidiMappingId, MidiMapping> _mappingsView;

  /// The mappings that are currently considered to be present.
  ///
  /// This is updated optimistically with respect to the back end. When create
  /// is called, the mapping is added, assuming that the backend will not error.
  /// If there is an error, the mapping is removed.
  UnmodifiableMapView<MidiMappingId, MidiMapping> get mappings => _mappingsView;

  MessageTransport<MidiApiMessage, MidiApiMessage> websocket;
  late StreamSubscription<MidiApiMessage> _subscription;

  Future<void> getMapping() async {
    _log.info('Initialising');
    const message = MidiApiMessage.getRequest();

    __mappings.clear();
    websocket.sink.add(message);
  }

  Future<void> createMapping(
    MidiMapping mapping,
  ) async {
    final response = websocket.stream
        .where((event) => event is MidiCreateResponse)
        .map((event) => event as MidiCreateResponse)
        .map(
          (event) => event.mapping,
        )
        .where((event) => event.mapping == mapping)
        .timeout(responseTimeout)
        .first;

    // FIXME: optimistically add the mapping to the UI state
    /*
    __mappings[mapping.id] = mapping.mapping;
    notifyListeners();
     */

    final message = MidiApiMessage.createRequest(mapping: mapping);
    websocket.sink.add(message);

    try {
      final responseMapping = await response;
      __mappings[responseMapping.id] = responseMapping.mapping;
      notifyListeners();
    } on TimeoutException {
      // FIXME: rollback optimistic update
      /*
      __mappings.remove(mapping.id);
      notifyListeners();
       */
    }
  }

  Future<void> updateMapping(MidiMappingEntry mapping) async {
    __mappings[mapping.id] = mapping.mapping;
    notifyListeners();

    final message = MidiApiMessage.update(mapping: mapping);
    websocket.sink.add(message);
  }

  Future<void> deleteMapping({required MidiMappingId id}) async {
    __mappings.remove(id);
    notifyListeners();

    final message = MidiApiMessage.remove(id: id);
    websocket.sink.add(message);
  }

  void _handleMessage(MidiApiMessage message) {
    switch (message) {
      case MidiGetRequest():
        break;
      case MidiCreateRequest():
        break;
      case MidiCreateResponse():
        break;
      case MidiUpdate(:final mapping):
        __mappings[mapping.id] = mapping.mapping;
        notifyListeners();
      case MidiRemove():
        break;
      case MidiMessageReceived():
        break;
    }
  }

  @override
  void dispose() {
    super.dispose();
    unawaited(_subscription.cancel());
  }
}
