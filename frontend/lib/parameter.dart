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

import 'json_websocket.dart';

part 'parameter.g.dart';

part 'parameter.freezed.dart';

final _log = Logger('shrapnel.parameter');

@freezed
class AudioParameterDoubleData with _$AudioParameterDoubleData {
  const factory AudioParameterDoubleData({
    required String id,
    required double value,
  }) = _AudioParameterDoubleData;

  factory AudioParameterDoubleData.fromJson(Map<String, dynamic> json) =>
      _$AudioParameterDoubleDataFromJson(json);
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

@Freezed(unionKey: 'messageType')
sealed class ParameterServiceOutputMessage
    with _$ParameterServiceOutputMessage {
  @FreezedUnionValue('initialiseParameters')
  factory ParameterServiceOutputMessage.requestInitialisation() =
      ParameterServiceOutputMessageRequestInitialisation;

  factory ParameterServiceOutputMessage.parameterUpdate({
    required String id,
    required double value,
  }) = ParameterServiceOutputMessageParameterUpdate;

  factory ParameterServiceOutputMessage.fromJson(Map<String, dynamic> json) =>
      _$ParameterServiceOutputMessageFromJson(json);
}

sealed class ParameterServiceInputMessage {
  ParameterServiceInputMessage();

  factory ParameterServiceInputMessage.fromJson(Map<String, dynamic> json) {
    final messageType = json['messageType'];
    return switch (messageType) {
      'parameterUpdate' =>
        ParameterServiceInputMessageParameterUpdate.fromJson(json),
      _ => throw ArgumentError("messageType '$messageType' is not known"),
    };
  }
}

class ParameterServiceInputMessageParameterUpdate
    extends ParameterServiceInputMessage {
  ParameterServiceInputMessageParameterUpdate({required this.parameter});

  factory ParameterServiceInputMessageParameterUpdate.fromJson(
    Map<String, dynamic> json,
  ) {
    if (json['messageType'] != 'parameterUpdate') {
      throw ArgumentError("messageType is not 'parameterUpdate'");
    }

    return ParameterServiceInputMessageParameterUpdate(
      parameter: AudioParameterDoubleData.fromJson(json),
    );
  }

  AudioParameterDoubleData parameter;
}

abstract class ParameterRepositoryBase {
  bool get isAlive;

  void sendMessage(ParameterServiceOutputMessage message);

  Stream<ParameterServiceInputMessage> get stream;

  Stream<void> get connectionStream;
}

class ParameterRepository implements ParameterRepositoryBase {
  ParameterRepository({required this.websocket});

  JsonWebsocket websocket;

  @override
  Stream<void> get connectionStream => websocket.connectionStream;

  @override
  bool get isAlive => websocket.isAlive;

  @override
  void sendMessage(ParameterServiceOutputMessage message) {
    websocket.send(message.toJson());
  }

  @override
  Stream<ParameterServiceInputMessage> get stream =>
      websocket.dataStream.transform(
        StreamTransformer.fromBind((input) async* {
          await for (final event in input) {
            yield ParameterServiceInputMessage.fromJson(event);
          }
        }),
      );
}

class ParameterService extends ChangeNotifier {
  ParameterService({required ParameterRepositoryBase repository})
      : _repository = repository {
    // TODO is this adding noticable latency when adjusting parameters?
    _sink.stream
        .throttleTime(
          const Duration(milliseconds: 100),
          trailing: true,
          leading: false,
        )
        .listen(_repository.sendMessage);

    _repository.stream.listen(_handleIncomingMessage);

    if (_repository.isAlive) {
      _requestParameterInitialisation();
    }
    _repository.connectionStream
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

  final ParameterRepositoryBase _repository;

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
      case ParameterServiceInputMessageParameterUpdate(:final parameter):
        if (!_parameters.containsKey(parameter.id)) {
          _log.warning("Couldn't find parameter with id ${parameter.id}");
          for (final id in _parameters.keys) {
            _log.warning(id);
          }
          return;
        }

        _parameters[parameter.id]!.setValue(parameter.value);
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
