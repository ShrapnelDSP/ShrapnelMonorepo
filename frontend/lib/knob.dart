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
    final _angle = (minAngle + _normalisedValue * sweepAngle) * 2 * m.pi / 360;
    print('Knob size $size');

    return Transform.rotate(
      angle: m.pi / 4,
      child: GestureDetector(
        onVerticalDragUpdate: (DragUpdateDetails details) {
          final changeInY = details.delta.dy;
          final changeInValue = distanceToAngle * -changeInY;
          final newValue = value + changeInValue;

          onChanged(newValue.clamp(min, max));
        },
        child: Transform.rotate(
          angle: -m.pi / 4,
          child: CustomPaint(
            painter: KnobArc(
                minAngle: minAngle * 2 * m.pi / 360,
                maxAngle: maxAngle * 2 * m.pi / 360,
                currentAngle: _angle,
                arcWidth: size * 0.08,
                backgroundColor: Theme.of(context).colorScheme.background,
                primaryColor: Theme.of(context).colorScheme.primary,
                ),
            child: SizedBox(
              height: size,
              width: size,
              child: Center(
                child: Container(
                  decoration: BoxDecoration(
                    color: Theme.of(context).colorScheme.background,
                    shape: BoxShape.circle,
                  ),
                  child: Transform.rotate(
                    angle: _angle,
                    child: Icon(
                      Icons.arrow_upward,
                      color: Theme.of(context).colorScheme.primary,
                      size: size * 0.8,
                    ),
                  ),
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class KnobArc extends CustomPainter {
  KnobArc({required this.arcWidth,
      required this.minAngle,
      required this.maxAngle,
      required this.currentAngle,
      required this.backgroundColor,
      required this.primaryColor,});

  final double minAngle;
  final double maxAngle;
  final double currentAngle;
  final double arcWidth;
  final Color primaryColor;
  final Color backgroundColor;
  late double sweepAngle = maxAngle - minAngle;

  @override
  void paint(Canvas canvas, Size size) {
    final background = Paint()
      ..color = backgroundColor
      ..style = PaintingStyle.stroke
      ..strokeWidth = arcWidth;

    final primary = Paint()
      ..color = primaryColor
      ..style = PaintingStyle.stroke
      ..strokeWidth = arcWidth;

    final bounds = Offset.zero & size;

    canvas.drawArc(
        bounds, -m.pi / 2 + minAngle, maxAngle - minAngle, false, background);
    canvas.drawArc(
        bounds, -m.pi / 2 + minAngle, currentAngle - minAngle, false, primary);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    // TODO: repaint only if something changed
    return true;
  }
}
