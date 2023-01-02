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
import 'package:json_annotation/json_annotation.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/transformers.dart';

import 'robust_websocket.dart';

part 'parameter.g.dart';

final log = Logger('shrapnel.parameter');

// TODO do some testing on this, should fail to create when either field is
// missing, wrong data type etc.
@JsonSerializable()
class AudioParameterDouble {
  AudioParameterDouble({
    required this.value,
    required this.id,
  });

  factory AudioParameterDouble.fromJson(Map<String, dynamic> json) =>
      _$AudioParameterDoubleFromJson(json);
  Map<String, dynamic> toJson() => _$AudioParameterDoubleToJson(this);

  final String id;
  final double value;
}

class AudioParameterDoubleModel extends ChangeNotifier {
  AudioParameterDoubleModel({
    required this.groupName,
    required this.name,
    required this.id,
    required this.parameterService,
  }) {
    parameterService.registerParameter(this);
  }

  @protected
  double _value = 0.5;

  final String groupName;
  final String name;
  final String id;
  final ParameterService parameterService;

  void onUserChanged(double value) {
    /* setting value instead of _value to make sure listeners are notified */
    this.value = value;
    parameterService.sink.add(toJson());
  }

  set value(double value) {
    _value = value;
    notifyListeners();
  }

  double get value => _value;

  String toJson() {
    final message = AudioParameterDouble(value: value, id: id).toJson();
    message['messageType'] = 'parameterUpdate';
    return json.encode(message);
  }
}

class ParameterService extends ChangeNotifier {
  ParameterService({required this.websocket}) {
    // TODO is this adding noticable latency when adjusting parameters?
    sink.stream
        .throttleTime(
          const Duration(milliseconds: 100),
          trailing: true,
          leading: false,
        )
        .listen(websocket.sendMessage);

    websocket.dataStream.listen(_handleIncomingEvent);

    if (websocket.isAlive) {
      _requestParameterInitialisation();
    }
    websocket.connectionStream.listen((_) => _requestParameterInitialisation());
  }

  void _requestParameterInitialisation() {
    final message = <String, dynamic>{};
    message['messageType'] = 'initialiseParameters';
    sink.add(json.encode(message));
  }

  final sink = StreamController<String>.broadcast();

  final _parameters = <AudioParameterDoubleModel>[];

  final RobustWebsocket websocket;

  void registerParameter(AudioParameterDoubleModel parameter) {
    _parameters.add(parameter);
  }

  void _handleIncomingEvent(dynamic event) {
    if (event is! String) {
      log.warning('Dropped message with unexpected type ${json.runtimeType}');
      return;
    }

    log.fine(event);

    final eventJson = json.decode(event) as Map<String, dynamic>;

    if (eventJson['messageType'] != 'parameterUpdate') {
      return;
    }

    final parameterToUpdate = AudioParameterDouble.fromJson(
      eventJson,
    );

    // TODO use a map for the parameters for O(1) lookup here
    for (final p in _parameters) {
      if (p.id == parameterToUpdate.id) {
        p.value = parameterToUpdate.value;
        return;
      }
    }

    log.warning("Couldn't find parameter with id ${parameterToUpdate.id}");
  }

  Map<String, String> get parameterNames {
    return Map.fromEntries(
      _parameters.map(
        (param) => MapEntry(param.id, '${param.groupName}: ${param.name}'),
      ),
    );
  }

  @override
  void dispose() {
    unawaited(sink.close());
    super.dispose();
  }
}
