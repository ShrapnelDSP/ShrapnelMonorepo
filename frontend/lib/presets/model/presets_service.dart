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

import 'current_parameters.dart';
import 'presets.dart';
import 'presets_repository.dart';
import 'selected_preset_repository.dart';

final _log = Logger('presets_fake');

abstract class PresetsRepositoryBase {
  Future<PresetRecord> create(PresetState preset);

  Future<void> update(PresetRecord preset);

  Future<void> delete(int id);

  ValueStream<Map<int, PresetRecord>> get presets;
}

abstract class SelectedPresetRepositoryBase {
  /// Load an existing preset.
  ///
  /// [id] must be the ID of one of the existing presets.
  Future<void> selectPreset(int id);

  ValueStream<int> get selectedPreset;
}

final presetsServiceProvider =
    AutoDisposeStateNotifierProvider<PresetsService, PresetsState>(
  (ref) {
    final service = PresetsService(
      presetsRepository: ref.read(presetsRepositoryProvider),
      selectedPresetRepository: ref.read(selectedPresetRepositoryProvider),
      parametersState: ref.read(currentParametersProvider),
    );

    ref.listen(currentParametersProvider, (_, next) {
      service._parametersState = next;
      service._updateState();
    });

    return service;
  },
);

class PresetsService extends StateNotifier<PresetsState>
    implements PresetsServiceBase {
  PresetsService({
    required this.presetsRepository,
    required this.selectedPresetRepository,
    required AsyncValue<PresetParametersData> parametersState,
  })  : _parametersState = parametersState,
        super(PresetsState.loading()) {
    _presetsSubscription =
        presetsRepository.presets.listen((_) => _updateState());
    _selectedPresetSubscription =
        selectedPresetRepository.selectedPreset.listen((_) => _updateState());
    _updateState();
  }

  AsyncValue<PresetParametersData> _parametersState;
  late final StreamSubscription<Map<int, PresetRecord>> _presetsSubscription;
  late final StreamSubscription<int> _selectedPresetSubscription;
  final PresetsRepositoryBase presetsRepository;
  final SelectedPresetRepositoryBase selectedPresetRepository;

  void _updateState() {
    final presets = presetsRepository.presets.valueOrNull;
    final selectedPreset = selectedPresetRepository.selectedPreset.valueOrNull;

    if (presets == null) {
      state = PresetsState.loading();
    } else {
      final sortedPresets = presets.values.toList()
        ..sort((a, b) => a.preset.name.compareTo(b.preset.name));
      state = PresetsState.ready(
        isCurrentModified: selectedPreset != null &&
            _parametersState.value !=
                presets[selectedPreset]?.preset.parameters,
        presets: sortedPresets,
        selectedPreset: selectedPreset,
        canUndo: false,
      );
    }
  }

  @override
  Future<void> create(String name) async {
    final parameters = _parametersState.value;
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

  @override
  Future<void> select(int id) async {
    await selectedPresetRepository.selectPreset(id);
  }

  @override
  Future<void> delete(int id) async {
    await presetsRepository.delete(id);
  }

  @override
  Future<void> saveChanges() async {
    final parameters = _parametersState.value;
    if (parameters == null) {
      throw StateError(
        'Attempted to save changes while parameters are loading',
      );
    }

    final currentPresetId = selectedPresetRepository.selectedPreset.valueOrNull;
    if (currentPresetId == null) {
      _log.severe("Don't know current preset id");
      return;
    }

    final currentPreset = presetsRepository.presets.value[currentPresetId];
    if (currentPreset == null) {
      _log.severe("Don't know preset with id $currentPresetId");
      return;
    }

    await presetsRepository.update(
      currentPreset.copyWith(
        preset: currentPreset.preset.copyWith(
          parameters: parameters,
        ),
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
    unawaited(_presetsSubscription.cancel());
    unawaited(_selectedPresetSubscription.cancel());
  }
}
