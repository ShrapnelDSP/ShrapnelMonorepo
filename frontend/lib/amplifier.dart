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
import 'package:provider/provider.dart';

import 'knob_with_label.dart';
import 'parameter.dart';

class AmplifierModel extends ChangeNotifier {
  AmplifierModel({required this.parameters});

  List<AudioParameterDoubleModel> parameters;
}

class Amplifier extends StatelessWidget {
  const Amplifier({
    Key? key,
    required this.name,
    required this.onTap,
    required this.full,
  }) : super(key: key);

  final bool full;
  final Function() onTap;

  final String name;

  List<Widget> knobs(BuildContext context, double scaleFactor) {
    final knobs = <Widget>[];
    final parameters = Provider.of<AmplifierModel>(context).parameters;

    for (var i = 0; i < parameters.length; i++) {
      knobs.add(ChangeNotifierProvider.value(
        value: parameters[i],
        child: KnobWithLabel(
          isEnabled: full,
          knobSize: scaleFactor * 25,
        ),
      ));
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
