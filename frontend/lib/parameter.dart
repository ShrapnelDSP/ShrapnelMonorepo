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

import 'package:flutter/widgets.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:riverpod_annotation/riverpod_annotation.dart';
import 'package:rxdart/rxdart.dart';

import 'api/api_websocket.dart';
import 'core/message_transport.dart';

part 'parameter.freezed.dart';

part 'parameter.g.dart';

final _log = Logger('shrapnel.parameter');

@freezed
class AudioParameterDoubleData with _$AudioParameterDoubleData {
  const factory AudioParameterDoubleData({
    required String id,
    required double value,
  }) = _AudioParameterDoubleData;
}

@freezed
class AudioParameterMetaData with _$AudioParameterMetaData {
  const factory AudioParameterMetaData({
    required String groupName,
    required String name,
    required String id,
  }) = _AudioParameterMetaData;
}

@riverpod
class AudioParameterDoubleModel extends _$AudioParameterDoubleModel {
  @override
  Stream<double> build(String parameterId) {
    return ref.watch(parameterServiceProvider).getParameter(parameterId);
  }

  void onUserChanged(double value) {
    _log.finest('user updated parameter $parameterId to $value');
    ref.read(parameterServiceProvider).parameterUpdatedByUser(
          AudioParameterDoubleData(value: value, id: parameterId),
        );
  }
}

@freezed
sealed class ParameterServiceOutputMessage
    with _$ParameterServiceOutputMessage {
  factory ParameterServiceOutputMessage.requestInitialisation() =
      ParameterServiceOutputMessageRequestInitialisation;

  factory ParameterServiceOutputMessage.parameterUpdate({
    required String id,
    required double value,
  }) = ParameterServiceOutputMessageParameterUpdate;
}

@freezed
sealed class ParameterServiceInputMessage with _$ParameterServiceInputMessage {
  factory ParameterServiceInputMessage.parameterUpdate({
    required String id,
    required double value,
  }) = ParameterServiceInputMessageParameterUpdate;
}

final parameterTransportProvider = Provider(
  (ref) => ParameterTransport(
    websocket: ref.read(apiWebsocketProvider),
  ),
);

class ParameterTransport
    implements
        MessageTransport<ParameterServiceOutputMessage,
            ParameterServiceInputMessage> {
  ParameterTransport({required this.websocket}) {
    _controller.stream.listen((message) {
      websocket.send(ApiMessage.parameterOutput(message: message));
    });
  }

  ApiWebsocket websocket;

  final _controller = StreamController<ParameterServiceOutputMessage>();

  @override
  Stream<void> get connectionStream => websocket.connectionStream;

  @override
  bool get isAlive => websocket.isAlive;

  @override
  Stream<ParameterServiceInputMessage> get stream => websocket.stream
      .whereType<ApiMessageParameterInput>()
      .map((event) => event.message);

  @override
  void dispose() {
    unawaited(_controller.close());
  }

  @override
  StreamSink<ParameterServiceOutputMessage> get sink => _controller;
}

final parameterServiceProvider = ChangeNotifierProvider(
  (ref) => ParameterService(
    transport: ref.read(parameterTransportProvider),
  ),
);

class ParameterService extends ChangeNotifier {
  ParameterService({
    required MessageTransport<ParameterServiceOutputMessage,
            ParameterServiceInputMessage>
        transport,
  }) : _transport = transport {
    unawaited(
      _transport.sink.addStream(
        _sink.stream.throttleTime(
          const Duration(milliseconds: 100),
          trailing: true,
          leading: false,
        ),
      ),
    );

    _transport.stream.listen(_handleIncomingMessage);

    if (_transport.isAlive) {
      _requestParameterInitialisation();
    }
    _transport.connectionStream
        .listen((_) => _requestParameterInitialisation());
  }

  void _requestParameterInitialisation() {
    _log.info('Initialising parameters');
    _sink.add(ParameterServiceOutputMessage.requestInitialisation());
  }

  final _parameterUpdatesController =
      StreamController<AudioParameterDoubleData>();

  /// Parameter updates performed by the user through the GUI
  late final Stream<AudioParameterDoubleData> parameterUpdates =
      _parameterUpdatesController.stream;
  final _sink = StreamController<ParameterServiceOutputMessage>.broadcast();

  final _parameters = <String, StreamController<double>>{};

  final MessageTransport<ParameterServiceOutputMessage,
      ParameterServiceInputMessage> _transport;

  Stream<double> getParameter(String parameterId) {
    return _parameters.putIfAbsent(parameterId, StreamController.new).stream;
  }

  void _handleIncomingMessage(ParameterServiceInputMessage message) {
    switch (message) {
      case ParameterServiceInputMessageParameterUpdate(:final id, :final value):
        if (!_parameters.containsKey(id)) {
          _log.warning("Couldn't find parameter with id $id");
          for (final id in _parameters.keys) {
            _log.warning(id);
          }
          return;
        }

        _parameters[id]!.add(value);
    }
  }

  @override
  void dispose() {
    unawaited(_sink.close());
    unawaited(_parameterUpdatesController.close());
    super.dispose();
  }

  void parameterUpdatedByUser(AudioParameterDoubleData parameter) {
    _parameters[parameter.id]!.add(parameter.value);
    _parameterUpdatesController.add(parameter);
    _sink.add(
      ParameterServiceOutputMessage.parameterUpdate(
        id: parameter.id,
        value: parameter.value,
      ),
    );
  }
}
