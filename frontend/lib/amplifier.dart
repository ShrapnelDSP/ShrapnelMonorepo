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

import 'package:flutter/material.dart';
import 'knob.dart';

class Amplifier extends StatelessWidget {
  const Amplifier({
    Key? key,
    required this.parameter,
    required this.onChanged,
    required this.parameterName,
    required this.name,
    required this.onTap,
    required this.full,
  }) : super(key: key);

  final List<double> parameter;
  final List<ValueChanged<double>> onChanged;
  final List<String> parameterName;

  final bool full;
  final Function() onTap;

  final String name;

  Widget knobWithLabel(int index, double scaleFactor) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Knob(
          value: parameter[index],
          onChanged:
              full ? onChanged[index] : (ignored) {/* not interactive */},
          size: scaleFactor * 25,
        ),
        if (full) const SizedBox(height: 10),
        if (full)
          Text(
            parameterName[index],
            textAlign: TextAlign.center,
          ),
      ],
    );
  }

  List<Widget> knobs(double scaleFactor) {
    final knobs = <Widget>[];

    for (var i = 0; i < parameter.length; i++) {
      knobs.add(knobWithLabel(i, scaleFactor));
      if (i < parameter.length - 1) {
        knobs.add(SizedBox(width: scaleFactor * 10));
      }
    }

    return knobs;
  }

  @override
  Widget build(BuildContext context) {
    final scaleFactor = full ? 3.0 : 1.0;

    return GestureDetector(
      onTap: onTap,
      child: Card(
        child: Container(
          margin: EdgeInsets.all(scaleFactor * 10),
          child: Row(
            children: knobs(scaleFactor),
          ),
        ),
      ),
    );
  }
}
