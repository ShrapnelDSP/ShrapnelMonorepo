import 'dart:async';
import 'dart:convert';

import 'package:flutter/widgets.dart';
import 'package:json_annotation/json_annotation.dart';
import 'package:logging/logging.dart';
import 'package:provider/provider.dart';
import 'package:rxdart/transformers.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

part 'parameter.g.dart';

final log = Logger('parameter');


// TODO do some testing on this, should fail to create when either field is
// missing, wrong data type etc.
@JsonSerializable()
class AudioParameterDouble {
    AudioParameterDouble({
        required this.value,
        required this.id,
    });

    factory AudioParameterDouble.fromJson(Map<String, dynamic> json) => _$AudioParameterDoubleFromJson(json);
    Map<String, dynamic> toJson() => _$AudioParameterDoubleToJson(this);

    @JsonKey(name: 'id')
    final String id;

    @JsonKey(name: 'value')
    final double value;
}

// TODO perhaps use composition to put an instance of AudioParameterDouble
// inside this model. That will allow us to use model.parameter.toJson to
// convert the parameter the model represents.
class AudioParameterDoubleModel extends ChangeNotifier {
  AudioParameterDoubleModel({
    required this.name,
    required this.id,
    required this.parameterService
  })
  {
      parameterService.registerParameter(this);
  }

  @protected
  double _value = 0.5;

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

  String toJson() => '{"id": "$id", "value": ${value.toStringAsFixed(2)}}';
}

class ParameterService extends ChangeNotifier {
  ParameterService() {
    // TODO is this adding noticable latency when adjusting parameters?
    channel.sink.addStream(sink.stream.throttleTime(
      const Duration(milliseconds: 100),
      trailing: true,
      leading: false,
    ));

    channel.stream.listen(_handleIncomingEvent);
  }

  final channel = WebSocketChannel.connect(
    Uri.parse('ws://guitar-dsp.local/websocket'),
  );

  final sink = StreamController<String>();

  final _parameters = <AudioParameterDoubleModel>[];

  void registerParameter(AudioParameterDoubleModel parameter) {
      _parameters.add(parameter);
  }

  void _handleIncomingEvent(dynamic event) {
      if(event is! String)
      {
          log.warning('Dropped message with unexpected type ${json.runtimeType}');
          return;
      }

      log.finer(event);

      final parameterToUpdate = AudioParameterDouble.fromJson(json.decode(event) as Map<String, dynamic>);

      log.finer(parameterToUpdate.id);
      log.finer(parameterToUpdate.value);

      for (final p in _parameters) {
          if(p.id == parameterToUpdate.id)
          {
            p.value = parameterToUpdate.value;
            return;
          }
      }

      log.warning("Couldn't find parameter with id ${parameterToUpdate.id}");
  }

  @override
  void dispose() {
    channel.sink.close();
    sink.close();
    super.dispose();
  }
}

class ParameterServiceProvider extends StatelessWidget {
  const ParameterServiceProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  final Widget child;

  @override
  Widget build(BuildContext context) => ChangeNotifierProvider(
        create: (_) => ParameterService(),
        child: child,
      );
}
