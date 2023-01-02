import 'dart:async';

import 'package:state_notifier/state_notifier.dart';

import '../../util/uuid.dart';
import 'midi_learn_state.dart';
import 'models.dart';
import 'service.dart';

class MidiLearnStateMachine extends StateNotifier<MidiLearnState> {
  MidiLearnStateMachine({required this.uuid, required this.service})
      : super(const MidiLearnState.idle());

  final MidiMappingService service;
  final Uuid uuid;

  void startLearning() {
    state.maybeWhen(
      idle: () => state = const MidiLearnState.waitForParameter(),
      orElse: () => throw StateError(
        'start learning called from unexpected state: $state',
      ),
    );
  }

  void parameterUpdated(String id) {
    state.maybeWhen(
      waitForParameter: () => state = MidiLearnState.waitForMidi(id),
      waitForMidi: (_) => state = MidiLearnState.waitForMidi(id),
      orElse: () {},
    );
  }

  void midiMessageReceived(MidiMessage message) {
    state.maybeWhen(
      waitForMidi: (parameterId) {
        message.maybeWhen(
          controlChange: (channel, control, value) {
            state = const MidiLearnState.idle();
            // TODO maybe add a loading state while this is in progress
            unawaited(
              service.createMapping(
                MidiMappingEntry(
                  id: uuid.v4(),
                  mapping: MidiMapping(
                    ccNumber: control,
                    midiChannel: channel,
                    parameterId: parameterId,
                    mode: MidiMappingMode.parameter,
                  ),
                ),
              ),
            );
          },
          orElse: () {},
        );
      },
      orElse: () {},
    );
  }

  void cancelLearning() {
    state = const MidiLearnState.idle();
  }
}
