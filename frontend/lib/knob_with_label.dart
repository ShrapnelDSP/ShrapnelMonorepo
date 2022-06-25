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

import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';

import 'knob.dart';
import 'parameter.dart';

class KnobWithLabel extends StatelessWidget {
  const KnobWithLabel({
    Key? key,
    required this.isEnabled,
    required this.knobSize,
  }) : super(key: key);

  final bool isEnabled;
  final double knobSize;

  @override
  Widget build(BuildContext context) {
    final parameter = Provider.of<AudioParameterDoubleModel>(context);

    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Knob(
          onChanged: isEnabled ? parameter.onUserChanged : (_) {},
          value: parameter.value,
          size: knobSize,
        ),
        if (isEnabled) const SizedBox(height: 10),
        if (isEnabled)
          Text(
            parameter.name,
            textAlign: TextAlign.center,
          ),
      ],
    );
  }
}

