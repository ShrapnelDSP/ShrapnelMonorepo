import 'dart:math' as m;

import 'package:flutter/material.dart';

class Knob extends StatelessWidget {
  const Knob({
    Key? key,
    required this.value,
    required this.onChanged,
    this.min = 0,
    this.max = 1,
    this.size = 50,
  }) : super(key: key);

  final double value;
  final double min;
  final double max;
  final double size;

  static const double minAngle = -160;
  static const double maxAngle = 160;
  static const double sweepAngle = maxAngle - minAngle;

  final ValueChanged<double> onChanged;

  @override
  Widget build(BuildContext context) {
    final distanceToAngle = 0.007 * (max - min);
    final _normalisedValue = (value - min) / (max - min);
    final _angle = minAngle + _normalisedValue * sweepAngle * 2 * m.pi / 360;

    return Transform.rotate(
      angle: m.pi / 4,
      child: GestureDetector(
        onVerticalDragUpdate: (DragUpdateDetails details) {
          final changeInY = details.delta.dy;
          final changeInValue = distanceToAngle * -changeInY;
          final newValue = value + changeInValue;
          final double clippedValue = m.min(m.max(newValue, min), max);

          onChanged(clippedValue);
        },
        child: Transform.rotate(
          angle: _angle - m.pi / 4,
          child: ClipOval(
            child: Container(
              color: Theme.of(context).colorScheme.onSurface,
              child: Icon(
                Icons.arrow_upward,
                color: Theme.of(context).colorScheme.primary,
                size: size,
              ),
            ),
          ),
        ),
      ),
    );
  }
}
