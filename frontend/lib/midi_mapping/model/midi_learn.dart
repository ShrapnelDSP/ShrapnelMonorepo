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

import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';

import '../../api/api_websocket.dart';
import '../../parameter.dart';
import 'midi_learn_state.dart';
import 'models.dart';
import 'service.dart';

final _log = Logger('shrapnel.midi_mapping.model.midi_learn');

final midiLearnServiceProvider =
    AutoDisposeStateNotifierProvider<MidiLearnServiceBase, MidiLearnState>(
  (ref) {
    return switch ((
      ref.watch(midiMappingServiceProvider),
      ref.watch(parameterServiceProvider),
      ref.watch(apiWebsocketProvider),
    )) {
      (final midiMappingService?, final parameterService?, final websocket?) =>
        MidiLearnService(
          mappingService: midiMappingService,
          parameterUpdates: parameterService.parameterUpdates.map((e) => e.id),
          midiMessages: websocket.stream
              .whereType<ApiMessageMidiMapping>()
              .map((event) => event.message)
              .whereType<MidiMessageReceived>()
              .map((event) => event.message),
        ),
      _ => MidiLearnServiceLoading(),
    };
  },
);

abstract class MidiLearnServiceBase extends StateNotifier<MidiLearnState> {
  MidiLearnServiceBase(super._state);

  void startLearning();

  void cancelLearning();

  Future<void> undoRemoveSimilarMappings();
}

class MidiLearnServiceLoading extends MidiLearnServiceBase {
  MidiLearnServiceLoading() : super(const MidiLearnState.loading());

  @override
  void cancelLearning() {
    throw StateError("Can't cancel learning while loading");
  }

  @override
  void startLearning() {
    throw StateError("Can't start learning while loading");
  }

  @override
  Future<void> undoRemoveSimilarMappings() {
    throw StateError("Can't undo learning while loading");
  }
}

class MidiLearnService extends MidiLearnServiceBase {
  MidiLearnService({
    required this.mappingService,
    required this.parameterUpdates,
    required this.midiMessages,
  }) : super(const MidiLearnState.idle(null)) {
    parameterUpdateSubscription = parameterUpdates.listen(parameterUpdated);
    midiMessageSubscription = midiMessages.listen(midiMessageReceived);
  }

  final MidiMappingService mappingService;
  final Stream<String> parameterUpdates;
  final Stream<MidiMessage> midiMessages;

  late final StreamSubscription<dynamic> parameterUpdateSubscription;
  late final StreamSubscription<dynamic> midiMessageSubscription;

  @override
  void startLearning() {
    _log.finest('startLearning');
    state.maybeWhen(
      idle: (_) => state = const MidiLearnState.waitForParameter(),
      orElse: () => null,
    );
  }

  void parameterUpdated(String id) {
    _log.finest('parameter updated: $id');
    state.maybeWhen(
      waitForParameter: () => state = MidiLearnState.waitForMidi(id),
      waitForMidi: (_) => state = MidiLearnState.waitForMidi(id),
      orElse: () {},
    );
  }

  void midiMessageReceived(MidiMessage message) {
    _log.finest('midiMessageReceived: $message');
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
                  List<MapEntry<MidiMappingId, MidiMapping>>.from(
                similarMappings,
              );

              if (similarMappingsList.isNotEmpty) {
                for (final mapping in similarMappingsList) {
                  _log.info('removing similar mapping: $mapping');
                  await mappingService.deleteMapping(id: mapping.key);
                }
              }

              await mappingService.createMapping(
                MidiMapping.parameter(
                  ccNumber: control,
                  midiChannel: channel,
                  parameterId: mappedParameterId,
                ),
              );

              if (!mounted) {
                return;
              }

              state = MidiLearnState.idle(similarMappingsList);
            },
            orElse: () => null,
          ),
        );
      },
      orElse: () {},
    );
  }

  @override
  void cancelLearning() {
    _log.finest('cancelLearning');
    state = const MidiLearnState.idle(null);
  }

  @override
  Future<void> undoRemoveSimilarMappings() async {
    _log.finest('undoRemoveSimilarMappings');
    unawaited(
      state.maybeWhen(
        idle: (duplicates) async {
          if (duplicates == null) {
            return;
          }

          state = const MidiLearnState.savingMapping();
          for (final mapping in duplicates) {
            await mappingService.createMapping(mapping.value);
          }
          state = const MidiLearnState.idle(null);
        },
        orElse: () => throw StateError(
          'undoRemoveSimilarMappings called in state: $state',
        ),
      ),
    );
  }

  @override
  void dispose() {
    unawaited(parameterUpdateSubscription.cancel());
    unawaited(midiMessageSubscription.cancel());
    super.dispose();
  }
}
