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

import '../../parameter.dart';
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
    StateNotifierProvider<PresetsService, PresetsState>(
  (ref) {
    final parameterService = ref.read(parameterServiceProvider);

    return PresetsService(
      presetsRepository: ref.read(presetsRepositoryProvider),
      selectedPresetRepository: ref.read(selectedPresetRepositoryProvider),
      parametersState: _ParametersMergeStream(
        ampGain: parameterService.getParameter('ampGain').value,
        ampChannel: parameterService.getParameter('ampChannel').value,
        bass: parameterService.getParameter('bass').value,
        middle: parameterService.getParameter('middle').value,
        treble: parameterService.getParameter('treble').value,
        contour: parameterService.getParameter('contour').value,
        volume: parameterService.getParameter('volume').value,
        noiseGateThreshold:
            parameterService.getParameter('noiseGateThreshold').value,
        noiseGateHysteresis:
            parameterService.getParameter('noiseGateHysteresis').value,
        noiseGateAttack: parameterService.getParameter('noiseGateAttack').value,
        noiseGateHold: parameterService.getParameter('noiseGateHold').value,
        noiseGateRelease:
            parameterService.getParameter('noiseGateRelease').value,
        noiseGateBypass: parameterService.getParameter('noiseGateBypass').value,
        chorusRate: parameterService.getParameter('chorusRate').value,
        chorusDepth: parameterService.getParameter('chorusDepth').value,
        chorusMix: parameterService.getParameter('chorusMix').value,
        chorusBypass: parameterService.getParameter('chorusBypass').value,
        wahPosition: parameterService.getParameter('wahPosition').value,
        wahVocal: parameterService.getParameter('wahVocal').value,
        wahBypass: parameterService.getParameter('wahBypass').value,
      ).stream,
    );
  },
);

class PresetsService extends StateNotifier<PresetsState>
    implements PresetsServiceBase {
  PresetsService({
    required this.presetsRepository,
    required this.selectedPresetRepository,
    required ValueStream<PresetParametersData> parametersState,
  })  : _parametersState = parametersState,
        super(PresetsState.loading()) {
    _subscription = _parametersState.listen((_) => _updateState());
    _presetsSubscription =
        presetsRepository.presets.listen((_) => _updateState());
    _selectedPresetSubscription =
        selectedPresetRepository.selectedPreset.listen((_) => _updateState());
    _updateState();
  }

  final ValueStream<PresetParametersData> _parametersState;
  late final StreamSubscription<PresetParametersData> _subscription;
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
    final record = await presetsRepository.create(
      PresetState(name: name, parameters: _parametersState.value),
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
          parameters: _parametersState.value,
        ),
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
    unawaited(_subscription.cancel());
    unawaited(_presetsSubscription.cancel());
    unawaited(_selectedPresetSubscription.cancel());
  }
}

class _ParametersMergeStream {
  _ParametersMergeStream({
    required this.ampGain,
    required this.ampChannel,
    required this.bass,
    required this.middle,
    required this.treble,
    required this.contour,
    required this.volume,
    required this.noiseGateThreshold,
    required this.noiseGateHysteresis,
    required this.noiseGateAttack,
    required this.noiseGateHold,
    required this.noiseGateRelease,
    required this.noiseGateBypass,
    required this.chorusRate,
    required this.chorusDepth,
    required this.chorusMix,
    required this.chorusBypass,
    required this.wahPosition,
    required this.wahVocal,
    required this.wahBypass,
  }) {
    _controller =
        BehaviorSubject<PresetParametersData>.seeded(getParametersState());

    void updateState() {
      _controller.add(getParametersState());
    }

    ampGain.listen((_) => updateState());
    ampChannel.listen((_) => updateState());
    bass.listen((_) => updateState());
    middle.listen((_) => updateState());
    treble.listen((_) => updateState());
    contour.listen((_) => updateState());
    volume.listen((_) => updateState());
    noiseGateThreshold.listen((_) => updateState());
    noiseGateHysteresis.listen((_) => updateState());
    noiseGateAttack.listen((_) => updateState());
    noiseGateHold.listen((_) => updateState());
    noiseGateRelease.listen((_) => updateState());
    noiseGateBypass.listen((_) => updateState());
    chorusRate.listen((_) => updateState());
    chorusDepth.listen((_) => updateState());
    chorusMix.listen((_) => updateState());
    chorusBypass.listen((_) => updateState());
    wahPosition.listen((_) => updateState());
    wahVocal.listen((_) => updateState());
    wahBypass.listen((_) => updateState());
  }

  late final BehaviorSubject<PresetParametersData> _controller;

  final ValueStream<double> ampGain;
  final ValueStream<double> ampChannel;
  final ValueStream<double> bass;
  final ValueStream<double> middle;
  final ValueStream<double> treble;
  final ValueStream<double> contour;
  final ValueStream<double> volume;
  final ValueStream<double> noiseGateThreshold;
  final ValueStream<double> noiseGateHysteresis;
  final ValueStream<double> noiseGateAttack;
  final ValueStream<double> noiseGateHold;
  final ValueStream<double> noiseGateRelease;
  final ValueStream<double> noiseGateBypass;
  final ValueStream<double> chorusRate;
  final ValueStream<double> chorusDepth;
  final ValueStream<double> chorusMix;
  final ValueStream<double> chorusBypass;
  final ValueStream<double> wahPosition;
  final ValueStream<double> wahVocal;
  final ValueStream<double> wahBypass;

  ValueStream<PresetParametersData> get stream => _controller.stream;

  PresetParametersData getParametersState() {
    final parameters = PresetParametersData(
      ampGain: ampGain.value,
      ampChannel: ampChannel.value,
      bass: bass.value,
      middle: middle.value,
      treble: treble.value,
      contour: contour.value,
      volume: volume.value,
      noiseGateThreshold: noiseGateThreshold.value,
      noiseGateHysteresis: noiseGateHysteresis.value,
      noiseGateAttack: noiseGateAttack.value,
      noiseGateHold: noiseGateHold.value,
      noiseGateRelease: noiseGateRelease.value,
      noiseGateBypass: noiseGateBypass.value,
      chorusRate: chorusRate.value,
      chorusDepth: chorusDepth.value,
      chorusMix: chorusMix.value,
      chorusBypass: chorusBypass.value,
      wahPosition: wahPosition.value,
      wahVocal: wahVocal.value,
      wahBypass: wahBypass.value,
    );

    return parameters;
  }
}
