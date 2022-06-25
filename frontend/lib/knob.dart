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
          angle: _angle - m.pi / 4,
          child: Container(
            decoration: BoxDecoration(
              color: Theme.of(context).colorScheme.background,
              shape: BoxShape.circle,
            ),
            child: Icon(
              Icons.arrow_upward,
              color: Theme.of(context).colorScheme.primary,
              size: size,
            ),
          ),
        ),
      ),
    );
  }
}
