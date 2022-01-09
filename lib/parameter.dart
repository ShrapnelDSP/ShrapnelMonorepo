import 'dart:async';

import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class AudioParameterDouble extends ChangeNotifier {
  AudioParameterDouble({
    required this.name,
    required this.id,
    required this.parameterChannel
  });

  @protected
  double _value = 0.5;

  final String name;
  final String id;
  final ParameterChannel parameterChannel;

  void onUserChanged(double value) {
      /* setting value instead of _value to make sure listeners are notified */
      this.value = value;
      parameterChannel.sink.add(toJson());
  }

  set value(double value) {
    _value = value;
    notifyListeners();
  }

  double get value => _value;

  String toJson() => '{"id": "$id", "value": ${value.toStringAsFixed(2)}}';
}

class ParameterChannel extends ChangeNotifier {
  ParameterChannel() {
      channel.sink.addStream(sink.stream);
  }

  final channel = WebSocketChannel.connect(
    Uri.parse('ws://guitar-dsp.local/websocket'),
  );

  final sink = StreamController<String>();

  @override
  void dispose() {
    channel.sink.close();
    sink.close();
    super.dispose();
  }
}

class ParameterChannelProvider extends StatelessWidget {
  const ParameterChannelProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  final Widget child;

  @override
  Widget build(BuildContext context) => ChangeNotifierProvider(
        create: (_) => ParameterChannel(),
        child: child,
      );
}
