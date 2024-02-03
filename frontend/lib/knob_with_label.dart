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
import 'package:flutter_riverpod/flutter_riverpod.dart';

import 'knob.dart';
import 'midi_mapping/model/midi_learn.dart';
import 'parameter.dart';
import 'parameters_meta.dart';

class KnobWithLabel extends ConsumerWidget {
  const KnobWithLabel({
    super.key,
    required this.parameterId,
    required this.isEnabled,
    required this.knobSize,
  });

  final String parameterId;
  final bool isEnabled;
  final double knobSize;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final value =
        ref.watch(audioParameterDoubleModelProvider(parameterId)).value ?? 0.5;
    final parameter =
        ref.read(audioParameterDoubleModelProvider(parameterId).notifier);
    final learningState = ref.watch(midiLearnServiceProvider);

    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        AnimatedTheme(
          data: Theme.of(context).copyWith(
            colorScheme: learningState.maybeWhen(
              waitForMidi: (id) => id == parameterId
                  ? Theme.of(context)
                      .colorScheme
                      .copyWith(primary: Colors.white)
                  : null,
              orElse: () => null,
            ),
          ),
          child: Knob(
            key: Key('knob-$parameterId'),
            onChanged: isEnabled ? parameter.onUserChanged : (_) {},
            value: value,
            size: knobSize,
          ),
        ),
        if (isEnabled) const SizedBox(height: 5),
        if (isEnabled)
          Text(
            ref.watch(audioParameterMetadataProvider(parameterId)).name,
            textAlign: TextAlign.center,
          ),
      ],
    );
  }
}
