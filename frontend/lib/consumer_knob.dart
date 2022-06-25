import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'knob.dart';
import 'parameter.dart';

class ConsumerKnob extends StatelessWidget {
  const ConsumerKnob({
      Key? key,
    this.min = 0,
    this.max = 1,
    this.size = 50,
  }) : super(key: key);

  final double min;
  final double max;
  final double size;

  @override
  Widget build(BuildContext context) {
          final parameter = Provider.of<AudioParameterDoubleModel>(context);
          return Knob(
                  onChanged: parameter.onUserChanged,
                  value: parameter.value,
                  min: min,
                  max: max,
                  size: size,
                  );
      }
}
