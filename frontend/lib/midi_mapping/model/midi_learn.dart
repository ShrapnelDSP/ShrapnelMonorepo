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

import 'dart:async';

import 'package:logging/logging.dart';
import 'package:state_notifier/state_notifier.dart';

import '../../util/uuid.dart';
import 'midi_learn_state.dart';
import 'models.dart';
import 'service.dart';

final _log = Logger('shrapnel.midi_mapping.model.midi_learn');

class MidiLearnService extends StateNotifier<MidiLearnState> {
  MidiLearnService({required this.uuid, required this.mappingService})
      : super(const MidiLearnState.idle(null));

  final MidiMappingService mappingService;
  final UuidService uuid;

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
      waitForMidi: (mappedParameterId) {
        unawaited(
          message.maybeMap(
            controlChange: (message) async {
              _log.info('Got MIDI: $message');

              state = const MidiLearnState.savingMapping();

              final channel = message.channel;
              final control = message.control;

              final similarMappings = mappingService.mappings.entries.map((e) {
                _log.fine(e);
                return e;
              }).where(
                (e) =>
                    (e.value.midiChannel == channel &&
                        e.value.ccNumber == control) ||
                    switch (e.value) {
                          MidiMappingToggle(:final parameterId) ||
                          MidiMappingParameter(:final parameterId) =>
                            parameterId,
                          MidiMappingButton() => null,
                        } ==
                        mappedParameterId,
              );

              // Need copy to prevent concurrent modification of the lazy iterator
              final similarMappingsList =
                  List<MapEntry<String, MidiMapping>>.from(similarMappings);

              if (similarMappingsList.isNotEmpty) {
                for (final mapping in similarMappingsList) {
                  _log.info('removing similar mapping: $mapping');
                  await mappingService.deleteMapping(id: mapping.key);
                }
              }

              await mappingService.createMapping(
                MidiMappingEntry(
                  id: uuid.v4(),
                  mapping: MidiMapping.parameter(
                    ccNumber: control,
                    midiChannel: channel,
                    parameterId: mappedParameterId,
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

  Future<void> undoRemoveSimilarMappings() async {
    unawaited(
      state.maybeWhen(
        idle: (duplicates) async {
          if (duplicates == null) {
            return;
          }

          state = const MidiLearnState.savingMapping();
          for (final mapping in duplicates) {
            await mappingService.createMapping(
              MidiMappingEntry(
                id: mapping.key,
                mapping: mapping.value,
              ),
            );
          }
          state = const MidiLearnState.idle(null);
        },
        orElse: () => throw StateError(
          'undoRemoveSimilarMappings called in state: $state',
        ),
      ),
    );
  }
}
