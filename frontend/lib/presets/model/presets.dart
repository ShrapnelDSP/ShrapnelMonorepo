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

import 'package:freezed_annotation/freezed_annotation.dart';

part 'presets.freezed.dart';

@freezed
class PresetParametersData with _$PresetParametersData {
  factory PresetParametersData({
    required double ampGain,
    required double ampChannel,
    required double bass,
    required double middle,
    required double treble,
    required double contour,
    required double volume,
    required double noiseGateThreshold,
    required double noiseGateHysteresis,
    required double noiseGateAttack,
    required double noiseGateHold,
    required double noiseGateRelease,
    required double noiseGateBypass,
    required double chorusRate,
    required double chorusDepth,
    required double chorusMix,
    required double chorusBypass,
    required double wahPosition,
    required double wahVocal,
    required double wahBypass,
  }) = _PresetParametersData;
}

@freezed
class PresetRecord with _$PresetRecord {
  factory PresetRecord({
    required int id,
    required PresetState preset,
  }) = _PresetRecord;
}

@freezed
class PresetState with _$PresetState {
  factory PresetState({
    required String name,
    required PresetParametersData parameters,
  }) = _PresetState;
}

@freezed
sealed class PresetsState with _$PresetsState {
  factory PresetsState.loading() = LoadingPresetsState;

  factory PresetsState.ready({
    required bool isCurrentModified,
    required bool canUndo,
    required List<PresetRecord> presets,
    required int? selectedPreset,
  }) = ReadyPresetsState;

  PresetsState._();
}
