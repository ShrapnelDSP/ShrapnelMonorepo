import 'dart:async';

import 'package:logging/logging.dart';
import 'package:state_notifier/state_notifier.dart';

import '../../util/uuid.dart';
import 'midi_learn_state.dart';
import 'models.dart';
import 'service.dart';

final _log = Logger('shrapnel.midi_mapping.model.midi_learn');

class MidiLearnStateMachine extends StateNotifier<MidiLearnState> {
  MidiLearnStateMachine({required this.uuid, required this.service})
      : super(const MidiLearnState.idle(null));

  final MidiMappingService service;
  final Uuid uuid;

  void startLearning() {
    state.maybeWhen(
      idle: (_) => state = const MidiLearnState.waitForParameter(),
      orElse: () => null,
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
        unawaited(
          message.maybeMap(
            controlChange: (message) async {
              _log.info('Got MIDI: $message');

              state = const MidiLearnState.savingMapping();

              final channel = message.channel;
              final control = message.control;

              final similarMappings = service.mappings.entries.map((e) {
                _log.fine(e);
                return e;
              }).where(
                (e) =>
                    e.value.midiChannel == channel &&
                        e.value.ccNumber == control ||
                    e.value.parameterId == parameterId,
              );

              // Need copy to prevent concurrent modification of the lazy iterator
              final similarMappingsList =
                  List<MapEntry<String, MidiMapping>>.from(similarMappings);

              if (similarMappingsList.isNotEmpty) {
                for (final mapping in similarMappingsList) {
                  _log.info('removing similar mapping: $mapping');
                  await service.deleteMapping(id: mapping.key);
                }
              }

              await service.createMapping(
                MidiMappingEntry(
                  id: uuid.v4(),
                  mapping: MidiMapping(
                    ccNumber: control,
                    midiChannel: channel,
                    parameterId: parameterId,
                    mode: MidiMappingMode.parameter,
                  ),
                ),
              );

              state = MidiLearnState.idle(similarMappingsList);
            },
            orElse: () => null,
          ),
        );
      },
      orElse: () {},
    );
  }

  void cancelLearning() {
    state = const MidiLearnState.idle(null);
  }

  Future<void> undoRemoveDuplicates() async {
    unawaited(
      state.maybeWhen(
        idle: (duplicates) async {
          if (duplicates == null) {
            return;
          }

          state = const MidiLearnState.savingMapping();
          for (final mapping in duplicates) {
            await service.createMapping(
              MidiMappingEntry(
                id: mapping.key,
                mapping: mapping.value,
              ),
            );
          }
          state = const MidiLearnState.idle(null);
        },
        orElse: () =>
            throw StateError('undoRemoveDuplicates called in state: $state'),
      ),
    );
  }

  @override
  Future<void> dispose() async {
    super.dispose();
  }
}
