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

import 'package:rxdart/rxdart.dart';

import 'presets.dart';
import 'presets_client.dart';
import 'presets_service.dart';

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

    final record = await client.presetUpdates
        // Don't check equality of the parameter values, they will have rounding
        // errors after roundtrip through the firmware, instead just check the
        // name.
        .firstWhere((element) => element.preset.name == preset.name)
        .timeout(const Duration(seconds: 2));

    final newValue = _presets.value..[record.id] = record;
    _presets.add(newValue);

    return record;
  }

  @override
  Future<void> delete(int id) async {
    await client.delete(id);

    final newValue = _presets.value..remove(id);
    _presets.add(newValue);
  }

  final _presets = BehaviorSubject.seeded(<int, PresetRecord>{});

  @override
  ValueStream<Map<int, PresetRecord>> get presets => _presets;

  @override
  Future<void> update(PresetRecord preset) async {
    await client.update(preset);

    final newValue = _presets.value..[preset.id] = preset;
    _presets.add(newValue);
  }

  void _handleNotification(PresetRecord preset) {
    _presets.add(_presets.value..[preset.id] = preset);
  }

  void dispose() {
    client.dispose();
    unawaited(_presets.close());
  }
}
