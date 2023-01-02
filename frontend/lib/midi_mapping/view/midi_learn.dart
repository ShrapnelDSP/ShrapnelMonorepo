import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../model/midi_learn.dart';
import '../model/midi_learn_state.dart';

class MidiLearnStatus extends StatelessWidget {
  const MidiLearnStatus({super.key});

  @override
  Widget build(BuildContext context) {
    final midiLearnState = context.watch<MidiLearnState>();
    final midiLearnStateMachine = context.read<MidiLearnStateMachine>();

    final isInProgress = midiLearnState.when(
      idle: () => false,
      waitForParameter: () => true,
      waitForMidi: (_) => true,
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
                  orElse: () => throw StateError('$midiLearnState'),
                )}',
              ),
              const SizedBox(
                width: 10,
              ),
              ElevatedButton(
                onPressed: midiLearnStateMachine.cancelLearning,
                child: const Text('Cancel'),
              )
            ],
          ),
        ),
      ),
    );
  }
}
