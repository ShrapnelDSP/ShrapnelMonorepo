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
import 'dart:convert';

import 'package:flutter/widgets.dart';
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';

import 'robust_websocket.dart';

part 'parameter.g.dart';
part 'parameter.freezed.dart';

final log = Logger('shrapnel.parameter');

@freezed
class AudioParameterDouble with _$AudioParameterDouble {
  const factory AudioParameterDouble({
    required String id,
    required double value,
  }) = _AudioParameterDouble;

  factory AudioParameterDouble.fromJson(Map<String, dynamic> json) =>
      _$AudioParameterDoubleFromJson(json);
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
    _controller.add(value);
    parameterService
        .parameterUpdatedByUser(AudioParameterDouble(value: value, id: id));
  }

  void setValue(double value) {
    _controller.add(value);
  }

  ValueStream<double> get value => _controller;
}

sealed class ParameterServiceOutputMessage {
  Map<String, dynamic> toJson();
}

class ParameterServiceOutputMessageRequestInitialisation
    extends ParameterServiceOutputMessage {
  @override
  Map<String, dynamic> toJson() => {
        'messageType': 'initialiseParameters',
      };
}

class ParameterServiceOutputMessageParameterUpdate
    extends ParameterServiceOutputMessage {
  ParameterServiceOutputMessageParameterUpdate({required this.parameter});

  AudioParameterDouble parameter;

  @override
  Map<String, dynamic> toJson() {
    final message = parameter.toJson();
    message['messageType'] = 'parameterUpdate';
    return message;
  }
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
      Map<String, dynamic> json,) {
    if (json['messageType'] != 'parameterUpdate') {
      throw ArgumentError("messageType is not 'parameterUpdate'");
    }

    return ParameterServiceInputMessageParameterUpdate(
      parameter: AudioParameterDouble.fromJson(json),
    );
  }

  AudioParameterDouble parameter;
}

abstract class ParameterClientBase {
  bool get isAlive;

  void sendMessage(ParameterServiceOutputMessage message);

  Stream<ParameterServiceInputMessage> get stream;

  Stream<void> get connectionStream;
}

class ParameterClient implements ParameterClientBase {
  ParameterClient({required this.websocket});

  RobustWebsocket websocket;

  @override
  Stream<void> get connectionStream => websocket.connectionStream;

  @override
  bool get isAlive => websocket.isAlive;

  @override
  void sendMessage(ParameterServiceOutputMessage message) {
    websocket.sendMessage(
      json.encode(message.toJson()),
    );
  }

  @override
  Stream<ParameterServiceInputMessage> get stream =>
      websocket.dataStream.transform(
        StreamTransformer.fromBind((input) async* {
          await for (final event in input) {
            if (event is! String) {
              log.warning(
                'Dropped message with unexpected type ${json.runtimeType}',
              );
              return;
            }

            log.fine(event);

            final eventJson = json.decode(event) as Map<String, dynamic>;
            yield ParameterServiceInputMessage.fromJson(eventJson);
          }
        }),
      );
}

class ParameterService extends ChangeNotifier {
  ParameterService({required ParameterClientBase transport})
      : _transport = transport {
    // TODO is this adding noticable latency when adjusting parameters?
    _sink.stream
        .throttleTime(
          const Duration(milliseconds: 100),
          trailing: true,
          leading: false,
        )
        .listen(_transport.sendMessage);

    _transport.stream.listen(_handleIncomingEvent);

    if (_transport.isAlive) {
      _requestParameterInitialisation();
    }
    _transport.connectionStream
        .listen((_) => _requestParameterInitialisation());
  }

  void _requestParameterInitialisation() {
    _sink.add(ParameterServiceOutputMessageRequestInitialisation());
  }

  final _parameterUpdatesController = StreamController<AudioParameterDouble>();
  late final Stream<AudioParameterDouble> parameterUpdates =
      _parameterUpdatesController.stream;
  final _sink = StreamController<ParameterServiceOutputMessage>.broadcast();

  final _parameters = <String, AudioParameterDoubleModel>{};

  final ParameterClientBase _transport;

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

  void _handleIncomingEvent(ParameterServiceInputMessage event) {
    switch (event) {
      case ParameterServiceInputMessageParameterUpdate(:final parameter):
        if (!_parameters.containsKey(parameter.id)) {
          log.warning("Couldn't find parameter with id ${parameter.id}");
          for (final id in _parameters.keys) {
            log.warning(id);
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

  void parameterUpdatedByUser(AudioParameterDouble parameter) {
    _parameterUpdatesController.add(parameter);
    _sink.add(
        ParameterServiceOutputMessageParameterUpdate(parameter: parameter),);
  }
}
