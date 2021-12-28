import 'package:flutter/material.dart';
import 'dart:math' as m;

class Knob extends StatelessWidget {
  final double value;
  final double min;
  final double max;

  static const double minAngle = -160;
  static const double maxAngle = 160;
  static const double sweepAngle = maxAngle - minAngle;

  final ValueChanged<double> onChanged;

  const Knob({
    Key? key,
    required this.value,
    required this.onChanged,
    this.min = 0,
    this.max = 1,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    double distanceToAngle = 0.007 * (max - min);
    double _normalisedValue = (value - min)/(max - min);
    double _angle = minAngle + _normalisedValue * sweepAngle * 2 * m.pi / 360;

    return Transform.rotate(
      angle: m.pi/4,
      child: GestureDetector(
        onVerticalDragUpdate: (DragUpdateDetails details) {
          double changeInY = details.delta.dy;
          double changeInValue = distanceToAngle * -changeInY;
          double newValue = value + changeInValue;
          double clippedValue = m.min(m.max(newValue, min), max);

          onChanged(clippedValue);
        },
        child: Transform.rotate(
          angle: _angle - m.pi/4,
          child: ClipOval(
            child: Container(
              color: Theme.of(context).colorScheme.surface,
              child: Icon(
                Icons.arrow_upward,
                color: Theme.of(context).colorScheme.onSurface,
                size: 50,
              ),
            ),
          ),
        ),
      ),
    );
  }
}
