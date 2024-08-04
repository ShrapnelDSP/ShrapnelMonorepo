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
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';
import 'package:shrapnel_common/api_data/api_message.dart';
import 'package:shrapnel_common/api_data/parameter_message.dart';

import 'api/api_websocket.dart';
import 'core/message_transport.dart';

part 'parameter.freezed.dart';

final _log = Logger('shrapnel.parameter');

@freezed
class AudioParameterDoubleData with _$AudioParameterDoubleData {
  const factory AudioParameterDoubleData({
    required String id,
    required double value,
  }) = _AudioParameterDoubleData;
}

class AudioParameterDoubleModel {
  AudioParameterDoubleModel({
    required this.groupName,
    required this.name,
    required this.id,
    required this.parameterService,
  }) {
    parameterService.registerParameter(this);
  }

  final _controller = BehaviorSubject<double>.seeded(0.5);

  final String groupName;
  final String name;
  final String id;
  final ParameterService parameterService;

  void onUserChanged(double value) {
    _log.finest('user updated parameter $id to $value');
    _controller.add(value);
    parameterService
        .parameterUpdatedByUser(AudioParameterDoubleData(value: value, id: id));
  }

  void setValue(double value) {
    _controller.add(value);
  }

  ValueStream<double> get value => _controller;
}

class ParameterTransport
    implements
        ReconnectingMessageTransport<ParameterServiceOutputMessage,
            ParameterServiceInputMessage> {
  ParameterTransport({required this.websocket}) {
    _controller.stream
        .map((message) => ApiMessage.parameterOutput(message: message))
        .listen(websocket.sink.add);
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

class ParameterService extends ChangeNotifier {
  ParameterService({
    required ReconnectingMessageTransport<ParameterServiceOutputMessage,
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
  late final Stream<AudioParameterDoubleData> parameterUpdates =
      _parameterUpdatesController.stream;
  final _sink = StreamController<ParameterServiceOutputMessage>.broadcast();

  final _parameters = <String, AudioParameterDoubleModel>{};

  final ReconnectingMessageTransport<ParameterServiceOutputMessage,
      ParameterServiceInputMessage> _transport;

  void registerParameter(AudioParameterDoubleModel parameter) {
    assert(!_parameters.containsKey(parameter.id));
    _parameters[parameter.id] = parameter;
  }

  AudioParameterDoubleModel getParameter(String parameterId) {
    if (!_parameters.containsKey(parameterId)) {
      throw StateError('Invalid parameter id: $parameterId');
    }

    return _parameters[parameterId]!;
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

        _parameters[id]!.setValue(value);
    }
  }

  Map<String, String> get parameterNames {
    return _parameters.map(
      (id, param) => MapEntry(id, '${param.groupName}: ${param.name}'),
    );
  }

  @override
  void dispose() {
    unawaited(_sink.close());
    unawaited(_parameterUpdatesController.close());
    super.dispose();
  }

  void parameterUpdatedByUser(AudioParameterDoubleData parameter) {
    _parameterUpdatesController.add(parameter);
    _sink.add(
      ParameterServiceOutputMessage.parameterUpdate(
        id: parameter.id,
        value: parameter.value,
      ),
    );
  }
}
