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
import 'package:riverpod_annotation/riverpod_annotation.dart';

import 'current_parameters.dart';
import 'presets.dart';
import 'presets_repository.dart';
import 'selected_preset_repository.dart';

part 'presets_service.g.dart';

final _log = Logger('presets_service');

abstract class PresetsRepositoryBase {
  Future<PresetRecord> create(PresetState preset);

  Future<void> update(PresetRecord preset);

  Future<void> delete(int id);

  Stream<Map<int, PresetRecord>> get presets;
}

abstract class SelectedPresetRepositoryBase {
  /// Load an existing preset.
  ///
  /// [id] must be the ID of one of the existing presets.
  Future<void> selectPreset(int id);

  Stream<int> get selectedPreset;
}

@riverpod
class PresetsService extends _$PresetsService {
  @override
  PresetsState build() {
    ref.onDispose(() {
      _log.warning('PresetsService dispose');
    });

    final presets = ref.watch(presetsStreamProvider).valueOrNull;
    final selectedPreset = ref.watch(selectedPresetStreamProvider).valueOrNull;
    final parametersState = ref.watch(currentParametersProvider).valueOrNull;

    _log
      ..finest('build')
      ..finest('presets=$presets')
      ..finest('selectedPreset=$selectedPreset')
      ..finest('presets[selectedPreset]=${presets?[selectedPreset]}')
      ..finest('parametersState=$parametersState');

    if (presets == null) {
      return PresetsState.loading();
    } else {
      final sortedPresets = presets.values.toList()
        ..sort((a, b) => a.preset.name.compareTo(b.preset.name));
      return PresetsState.ready(
        isCurrentModified: selectedPreset != null &&
            parametersState != presets[selectedPreset]?.preset.parameters,
        presets: sortedPresets,
        selectedPreset: selectedPreset,
        canUndo: false,
      );
    }
  }

  Future<void> create(String name) async {
    final parameters = ref.read(currentParametersProvider).value;
    final presetsRepository = ref.read(presetsRepositoryProvider);
    final selectedPresetRepository = ref.read(selectedPresetRepositoryProvider);

    if (parameters == null) {
      throw StateError(
        'Attempted to create preset while parameters are loading',
      );
    }

    final record = await presetsRepository.create(
      PresetState(name: name, parameters: parameters),
    );
    await selectedPresetRepository.selectPreset(record.id);
  }

  Future<void> select(int id) async {
    await ref.read(selectedPresetRepositoryProvider).selectPreset(id);
  }

  Future<void> delete(int id) async {
    await ref.read(presetsRepositoryProvider).delete(id);
  }

  Future<void> saveChanges() async {
    final parameters = ref.read(currentParametersProvider).valueOrNull;
    if (parameters == null) {
      throw StateError(
        'Attempted to save changes while parameters are loading',
      );
    }

    final currentPresetId = ref.read(selectedPresetStreamProvider).valueOrNull;
    if (currentPresetId == null) {
      _log.severe("Don't know current preset id");
      return;
    }

    final currentPreset =
        ref.read(presetsStreamProvider).valueOrNull?[currentPresetId];
    if (currentPreset == null) {
      _log.severe("Don't know preset with id $currentPresetId");
      return;
    }

    await ref.read(presetsRepositoryProvider).update(
          currentPreset.copyWith(
            preset: currentPreset.preset.copyWith(
              parameters: parameters,
            ),
          ),
        );
  }
}
