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
import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';

import 'knob.dart';
import 'midi_mapping/model/midi_learn_state.dart';
import 'parameter.dart';

class KnobWithLabel extends StatelessWidget {
  const KnobWithLabel({
    super.key,
    required this.isEnabled,
    required this.knobSize,
  });

  final bool isEnabled;
  final double knobSize;

  @override
  Widget build(BuildContext context) {
    final parameter = Provider.of<AudioParameterDoubleModel>(context);
    final learningState = context.watch<MidiLearnState>();

    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        AnimatedTheme(
          data: Theme.of(context).copyWith(
            colorScheme: learningState.maybeWhen(
              waitForMidi: (id) => id == parameter.id
                  ? Theme.of(context)
                      .colorScheme
                      .copyWith(primary: Colors.white)
                  : null,
              orElse: () => null,
            ),
          ),
          child: Knob(
            onChanged: isEnabled ? parameter.onUserChanged : (_) {},
            value: parameter.value,
            size: knobSize,
          ),
        ),
        if (isEnabled) const SizedBox(height: 5),
        if (isEnabled)
          Text(
            parameter.name,
            textAlign: TextAlign.center,
          ),
      ],
    );
  }
}
