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
PresetsRepositoryBase presetsRepository(PresetsRepositoryRef ref) =>
    PresetsRepository(
      client: ref.watch(presetsClientProvider),
    );

@riverpod
Stream<Map<int, PresetRecord>> presetsStream(PresetsStreamRef ref) =>
    ref.watch(presetsRepositoryProvider).presets;

class PresetsRepository implements PresetsRepositoryBase {
  PresetsRepository({required this.client}) {
    client.presetUpdates.listen(_handleNotification);
    client.connectionStream.listen((_) {
      unawaited(client.initialise());
    });
  }

  PresetsClient client;

  @override
  Future<PresetRecord> create(PresetState preset) async {
    await client.create(preset);

    // Don't check equality of the parameter values, they will have rounding
    // errors after roundtrip through the firmware, instead just check the
    // name.
    final record = await client.presetUpdates
        .firstWhere((element) => element.preset.name == preset.name)
        .timeout(const Duration(seconds: 2));

    _state = _state!..[record.id] = record;

    return record;
  }

  @override
  Future<void> delete(int id) async {
    await client.delete(id);

    _state = _state!..remove(id);
  }

  final _presets = StreamController<Map<int, PresetRecord>>();

  Map<int, PresetRecord>? __state;

  set _state(Map<int, PresetRecord>? newState) {
    __state = newState;
    _presets.add(newState!);
  }

  Map<int, PresetRecord>? get _state => __state;

  @override
  Stream<Map<int, PresetRecord>> get presets => _presets.stream;

  @override
  Future<void> update(PresetRecord preset) async {
    await client.update(preset);

    _state = _state!..[preset.id] = preset;
  }

  void _handleNotification(PresetRecord preset) {
    if (_state == null) {
      _state = {preset.id: preset};
    } else {
      _state = _state!..[preset.id] = preset;
    }
  }

  void dispose() {
    client.dispose();
    unawaited(_presets.close());
  }
}
