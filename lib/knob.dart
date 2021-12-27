import 'package:flutter/material.dart';
import 'dart:math';

class Knob extends StatefulWidget {
  final double value;
  final double min;
  final double max;

  final ValueChanged<double> onChanged;

  const Knob({
    Key? key,
    required this.value,
    required this.onChanged,
    this.min = 0,
    this.max = 1,
  }) : super(key: key);

  @override
  State<StatefulWidget> createState() => KnobState();
}

class KnobState extends State<Knob> {
  static const double minAngle = -160;
  static const double maxAngle = 160;
  static const double sweepAngle = maxAngle - minAngle;

  @override
  Widget build(BuildContext context) {
    double distanceToAngle = 0.007 * (widget.max - widget.min);
    double _normalisedValue = (widget.value - widget.min)/(widget.max - widget.min);
    double _angle = minAngle + (_normalisedValue*(sweepAngle)) * 2 * pi / 360;

    return GestureDetector(
      onVerticalDragUpdate: (DragUpdateDetails details) {
        double changeInY = details.delta.dy;
        double changeInValue = distanceToAngle * -changeInY;
        double newValue = widget.value + changeInValue;
        double clippedValue = min(max(newValue, widget.min), widget.max);

        widget.onChanged(clippedValue);
      },
      child: Transform.rotate(
        angle: _angle,
        child: ClipOval(
          child: Container(
            color: Colors.blue,
            child: const Icon(
              Icons.arrow_upward,
              color: Colors.white,
              size: 50,
            ),
          ),
        ),
      ),
    );
  }
}
