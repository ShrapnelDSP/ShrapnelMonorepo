import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';
import 'package:stream_channel/stream_channel.dart';

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

class ParameterUpdater<T extends AudioParameterDouble> extends StatelessWidget {
  final StreamChannel channel;

  const ParameterUpdater({
    Key? key,
    required this.channel,
  }) : super(key: key);

  @override
  build(BuildContext context) => Consumer<T>(
        builder: (context, parameter, _) {
          channel.sink.add(parameter.toJson());
          return const SizedBox.shrink();
        },
      );
}
