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
import 'package:vector_math/vector_math_64.dart' show Vector3;

import 'util/conditional_parent.dart';

const ColorFilter _greyscale = ColorFilter.matrix(<double>[
  ...[0.2126, 0.7152, 0.0722, 0, 0],
  ...[0.2126, 0.7152, 0.0722, 0, 0],
  ...[0.2126, 0.7152, 0.0722, 0, 0],
  ...[0, 0, 0, 1, 0],
]);

const ColorFilter _identity = ColorFilter.matrix(<double>[
  ...[1, 0, 0, 0, 0],
  ...[0, 1, 0, 0, 0],
  ...[0, 0, 1, 0, 0],
  ...[0, 0, 0, 1, 0],
]);

class Knob extends StatelessWidget {
  const Knob({
    super.key,
    required this.value,
    required this.defaultValue,
    required this.onChanged,
    this.min = 0,
    this.max = 1,
    this.size = 50,
  });

  final double? value;
  final double defaultValue;
  final double min;
  final double max;
  final double size;

  static const double minAngle = -0.85 * m.pi;
  static const double maxAngle = -minAngle;
  static const double sweepAngle = maxAngle - minAngle;

  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    final distanceToValue = 0.003 * (max - min);
    final normalisedValue = switch (value) {
      final value? => (value - min) / (max - min),
      null => null,
    };
    final angle = switch (normalisedValue) {
      final normalisedValue? => minAngle + normalisedValue * sweepAngle,
      null => -m.pi / 2,
    };

    return ColorFiltered(
      colorFilter: value == null ? _greyscale : _identity,
      child: Transform.rotate(
        angle: m.pi / 4,
        child: ConditionalParent(
          condition: onChanged != null,
          builder: (child) => GestureDetector(
            onVerticalDragUpdate: (DragUpdateDetails details) {
              if (value == null) {
                return;
              }

              final changeInY = details.delta.dy;
              final changeInValue = distanceToValue * -changeInY;
              final newValue = value! + changeInValue;

              // checked in condition
              onChanged!(newValue.clamp(min, max));
            },
            onDoubleTap: () {
              onChanged!(defaultValue);
            },
            child: child,
          ),
          child: Transform.rotate(
            angle: -m.pi / 4,
            child: ClipRect(
              child: CustomPaint(
                painter: _KnobArc(
                  minAngle: minAngle,
                  maxAngle: maxAngle,
                  currentAngle: angle,
                  arcWidth: size * 0.08,
                  backgroundColor: Theme.of(context).colorScheme.background,
                  primaryColor: Theme.of(context).colorScheme.primary,
                ),
                child: SizedBox(
                  height: size,
                  width: size,
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class _KnobArc extends CustomPainter {
  const _KnobArc({
    required this.arcWidth,
    required this.minAngle,
    required this.maxAngle,
    required this.currentAngle,
    required this.backgroundColor,
    required this.primaryColor,
  }) : super();

  final double minAngle;
  final double maxAngle;
  final double currentAngle;
  final double arcWidth;
  final Color primaryColor;
  final Color backgroundColor;

  @override
  void paint(Canvas canvas, Size size) {
    final background = Paint()
      ..color = backgroundColor
      ..style = PaintingStyle.stroke
      ..strokeWidth = arcWidth
      ..strokeCap = StrokeCap.round;

    final primary = Paint()
      ..color = primaryColor
      ..style = PaintingStyle.stroke
      ..strokeWidth = arcWidth
      ..strokeCap = StrokeCap.round;

    final bounds = Offset(arcWidth / 2, arcWidth / 2) &
        Size.fromRadius(size.width / 2 - arcWidth / 2);

    canvas.drawArc(
      bounds,
      -m.pi / 2 + minAngle,
      maxAngle - minAngle,
      false,
      background,
    );
    canvas.drawArc(
      bounds,
      -m.pi / 2 + minAngle,
      currentAngle - minAngle,
      false,
      primary,
    );

    background.style = PaintingStyle.fill;
    canvas.drawCircle(
      size.center(Offset.zero),
      size.width * 0.50 - arcWidth * 1.5,
      background,
    );

    // create the knob with tip at the origin
    var knob = Path()..relativeLineTo(0, size.width * 0.13);
    // move the tip up to near the edge of the inner circle
    knob = knob.transform(
      Matrix4.translation(Vector3(0, -size.width * 0.28, 0)).storage,
    );
    knob = knob.transform(Matrix4.rotationZ(currentAngle).storage);
    knob = knob.transform(
      Matrix4.translation(Vector3(size.width / 2, size.height / 2, 0)).storage,
    );

    canvas.drawPath(knob, primary);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    return true;
  }
}
