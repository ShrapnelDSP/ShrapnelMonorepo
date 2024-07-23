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

import 'package:riverpod_annotation/riverpod_annotation.dart';

import 'presets.dart';
import 'presets_client.dart';
import 'presets_service.dart';

part 'presets_repository.g.dart';

@riverpod
PresetsRepositoryBase? presetsRepository(PresetsRepositoryRef ref) =>
    switch (ref.watch(presetsClientProvider)) {
      final client? => PresetsRepository(
          client: client,
        ),
      null => null,
    };

@riverpod
Stream<Map<int, PresetRecord>> presetsStream(PresetsStreamRef ref) =>
    switch (ref.watch(presetsRepositoryProvider)) {
      final repository? => repository.presets,
      null => const Stream.empty(),
    };

class PresetsRepository implements PresetsRepositoryBase {
  PresetsRepository({required this.client}) {
    _initTimer = Timer(const Duration(seconds: 1), _initTimerExpired);

    client.presetUpdates.listen(_handleNotification);
    unawaited(client.initialise());
  }

  PresetsClient client;
  late Timer _initTimer;
  bool _isLoading = true;

  void _initTimerExpired() {
    // Firmware doesn't send any response when there are no presets configured.
    // It also doesn't send any message to indicate that initialisation is
    // complete. Assume initialisation completes in 1 second.
    _isLoading = false;

    // notify state using side-effect in setter
    // ignore: no_self_assignments
    _state = _state;
  }

  @override
  Future<PresetRecord> create(PresetState preset) async {
    await client.create(preset);

    // Don't check equality of the parameter values, they will have rounding
    // errors after roundtrip through the firmware, instead just check the
    // name.
    final record = await client.presetUpdates
        .firstWhere((element) => element.preset.name == preset.name)
        .timeout(const Duration(seconds: 2));

    _state = _state..[record.id] = record;

    return record;
  }

  @override
  Future<void> delete(int id) async {
    await client.delete(id);

    _state = _state..remove(id);
  }

  final _presets = StreamController<Map<int, PresetRecord>>();

  Map<int, PresetRecord> __state = {};

  set _state(Map<int, PresetRecord> newState) {
    __state = newState;

    if (_isLoading) {
      return;
    }
    _presets.add(newState);
  }

  Map<int, PresetRecord> get _state => __state;

  @override
  Stream<Map<int, PresetRecord>> get presets => _presets.stream;

  @override
  Future<void> update(PresetRecord preset) async {
    await client.update(preset);

    _state = _state..[preset.id] = preset;
  }

  void _handleNotification(PresetRecord preset) {
    _state = _state..[preset.id] = preset;
  }

  void dispose() {
    _initTimer.cancel();
    client.dispose();
    unawaited(_presets.close());
  }
}
