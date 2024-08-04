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

part 'presets_message.freezed.dart';

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
sealed class PresetsMessage with _$PresetsMessage {
  factory PresetsMessage.initialise() = PresetsMessageInitialise;

  factory PresetsMessage.notify(int id, PresetData preset) =
      PresetsMessageNotify;

  factory PresetsMessage.create(PresetData preset) = PresetsMessageCreate;

  factory PresetsMessage.update(int id, PresetData preset) =
      PresetsMessageUpdate;

  factory PresetsMessage.delete(int id) = PresetsMessageDelete;
}

@freezed
class PresetData with _$PresetData {
  factory PresetData({
    required String name,
    required PresetParametersData parameters,
  }) = _PresetData;
}
