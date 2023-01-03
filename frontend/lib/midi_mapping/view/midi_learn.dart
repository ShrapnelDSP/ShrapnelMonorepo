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
import 'package:flutter/scheduler.dart';
import 'package:provider/provider.dart';

import '../model/midi_learn.dart';
import '../model/midi_learn_state.dart';

class MidiLearnStatus extends StatelessWidget {
  const MidiLearnStatus({super.key});

  @override
  Widget build(BuildContext context) {
    final midiLearnState = context.watch<MidiLearnState>();
    final midiLearnService = context.read<MidiLearnService>();

    final isInProgress = midiLearnState.when(
      idle: (_) => false,
      waitForParameter: () => true,
      waitForMidi: (_) => true,
      savingMapping: () => true,
    );

    final isCancellable = midiLearnState.when(
      idle: (_) => false,
      waitForParameter: () => true,
      waitForMidi: (_) => true,
      savingMapping: () => false,
    );

    midiLearnState.maybeWhen(
      idle: (duplicates) {
        if (duplicates == null || duplicates.isEmpty) {
          return;
        }

        final snackBar = SnackBar(
          content: const Text('Some duplicate mappings were deleted'),
          action: SnackBarAction(
            label: 'Restore',
            onPressed: midiLearnService.undoRemoveSimilarMappings,
          ),
        );

        // Find the ScaffoldMessenger in the widget tree
        // and use it to show a SnackBar.
        SchedulerBinding.instance.addPostFrameCallback(
          (_) => ScaffoldMessenger.of(context).showSnackBar(snackBar),
        );
      },
      orElse: () {},
    );

    if (!isInProgress) {
      return const SizedBox.shrink();
    }

    return Padding(
      padding: const EdgeInsets.all(2),
      child: Card(
        child: Padding(
          padding: const EdgeInsets.all(6),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(
                'MIDI Learn: ${midiLearnState.maybeWhen(
                  waitForParameter: () => 'Move a knob',
                  waitForMidi: (_) => 'Move a MIDI controller',
                  savingMapping: () => 'Saving...',
                  orElse: () => throw StateError('$midiLearnState'),
                )}',
              ),
              const SizedBox(
                width: 10,
              ),
              if (isCancellable)
                ElevatedButton(
                  onPressed: midiLearnService.cancelLearning,
                  child: const Text('Cancel'),
                )
            ],
          ),
        ),
      ),
    );
  }
}
