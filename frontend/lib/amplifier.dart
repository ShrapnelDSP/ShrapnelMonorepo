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

import 'knob_with_label.dart';

abstract class AmplifierModel {
  String get name;

  List<String> get parameters;
}

class Amplifier extends StatelessWidget {
  const Amplifier({
    super.key,
    required this.model,
    required this.onTap,
    required this.full,
  });

  final AmplifierModel model;
  final bool full;
  final void Function() onTap;

  List<Widget> knobs(BuildContext context, double scaleFactor) {
    final knobs = <Widget>[];
    final parameters = model.parameters;

    for (var i = 0; i < parameters.length; i++) {
      knobs.add(
        KnobWithLabel(
          isEnabled: full,
          knobSize: scaleFactor * 25,
          parameterId: parameters[i],
        ),
      );
      if (i < parameters.length - 1) {
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
        child: Padding(
          padding: EdgeInsets.all(scaleFactor * 10),
          child: Row(
            mainAxisSize: MainAxisSize.min,
            children: knobs(context, scaleFactor),
          ),
        ),
      ),
    );
  }
}
