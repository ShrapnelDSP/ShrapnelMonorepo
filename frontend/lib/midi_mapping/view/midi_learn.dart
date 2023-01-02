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
