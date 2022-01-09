import 'dart:async';
import 'dart:convert';

import 'package:flutter/widgets.dart';
import 'package:logging/logging.dart';
import 'package:provider/provider.dart';
import 'package:rxdart/transformers.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

final log = Logger('parameter');

class AudioParameterDouble extends ChangeNotifier {
  AudioParameterDouble({
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

  final _parameters = <AudioParameterDouble>[];

  void registerParameter(AudioParameterDouble parameter) {
      _parameters.add(parameter);
  }

  void _handleIncomingEvent(dynamic event) {
      if(event is! String)
      {
          log.warning('Dropped message with unexpected type ${json.runtimeType}');
          return;
      }

      log.finer(event);

      final dynamic decoded = json.decode(event);
      final dynamic idToUpdate = decoded['id'];
      final dynamic value = decoded['value'];

      if(idToUpdate is! String)
      {
          log.severe('invalid id $idToUpdate');
          return;
      }

      if(value is! double)
      {
          log.severe('invalid value $idToUpdate');
          return;
      }

      for (final p in _parameters) {
          if(p.id == idToUpdate)
          {
            p.value = value;
            return;
          }
      }

      log.warning("Couldn't find parameter with id $idToUpdate");
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
