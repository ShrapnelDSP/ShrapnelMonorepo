import 'dart:async';

import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class AudioParameterDouble {
  @protected
  double value = 0.5;

  final String name;
  final String id;

  AudioParameterDouble({
    required this.name,
    required this.id,
  });

  String toJson() => '{"id": "$id", "value": ${value.toStringAsFixed(2)}}';
}

class ParameterUpdater<P extends AudioParameterDouble,
    C extends ParameterChannel> extends StatelessWidget {
  const ParameterUpdater({
    Key? key,
  }) : super(key: key);

  @override
  build(BuildContext context) => Consumer2<P, C>(
        builder: (context, parameter, channel, _) {
          channel.sink.add(parameter.toJson());
          return const SizedBox.shrink();
        },
      );
}

class ParameterChannel extends ChangeNotifier {
  final channel = WebSocketChannel.connect(
    Uri.parse('ws://guitar-dsp.local/websocket'),
  );

  StreamSink get sink => channel.sink;

  @override
  void dispose() {
    channel.sink.close();
    super.dispose();
  }
}

class ParameterChannelProvider extends StatelessWidget {
  final Widget child;

  const ParameterChannelProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) => ChangeNotifierProvider(
        create: (_) => ParameterChannel(),
        child: child,
      );
}
