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

import 'package:riverpod_annotation/riverpod_annotation.dart';

import '../../parameter.dart';
import 'presets.dart';

part 'current_parameters.g.dart';

@riverpod
AsyncValue<PresetParametersData> currentParameters(CurrentParametersRef ref) {
  double? watchParameterValue(String id) {
    return ref
        .watch(audioParameterDoubleModelProvider(id))
        .unwrapPrevious()
        .valueOrNull;
  }

  final ampGain = watchParameterValue('ampGain');
  final ampChannel = watchParameterValue('ampChannel');
  final bass = watchParameterValue('bass');
  final middle = watchParameterValue('middle');
  final treble = watchParameterValue('treble');
  final contour = watchParameterValue('contour');
  final volume = watchParameterValue('volume');
  final noiseGateThreshold = watchParameterValue('noiseGateThreshold');
  final noiseGateHysteresis = watchParameterValue('noiseGateHysteresis');
  final noiseGateAttack = watchParameterValue('noiseGateAttack');
  final noiseGateHold = watchParameterValue('noiseGateHold');
  final noiseGateRelease = watchParameterValue('noiseGateRelease');
  final noiseGateBypass = watchParameterValue('noiseGateBypass');
  final chorusRate = watchParameterValue('chorusRate');
  final chorusDepth = watchParameterValue('chorusDepth');
  final chorusMix = watchParameterValue('chorusMix');
  final chorusBypass = watchParameterValue('chorusBypass');
  final wahPosition = watchParameterValue('wahPosition');
  final wahVocal = watchParameterValue('wahVocal');
  final wahBypass = watchParameterValue('wahBypass');

  return switch ((
    ampGain,
    ampChannel,
    bass,
    middle,
    treble,
    contour,
    volume,
    noiseGateThreshold,
    noiseGateHysteresis,
    noiseGateAttack,
    noiseGateHold,
    noiseGateRelease,
    noiseGateBypass,
    chorusRate,
    chorusDepth,
    chorusMix,
    chorusBypass,
    wahPosition,
    wahVocal,
    wahBypass,
  )) {
    (
      final ampGain?,
      final ampChannel?,
      final bass?,
      final middle?,
      final treble?,
      final contour?,
      final volume?,
      final noiseGateThreshold?,
      final noiseGateHysteresis?,
      final noiseGateAttack?,
      final noiseGateHold?,
      final noiseGateRelease?,
      final noiseGateBypass?,
      final chorusRate?,
      final chorusDepth?,
      final chorusMix?,
      final chorusBypass?,
      final wahPosition?,
      final wahVocal?,
      final wahBypass?,
    ) =>
      AsyncValue.data(
        PresetParametersData(
          ampGain: ampGain,
          ampChannel: ampChannel,
          bass: bass,
          middle: middle,
          treble: treble,
          contour: contour,
          volume: volume,
          noiseGateThreshold: noiseGateThreshold,
          noiseGateHysteresis: noiseGateHysteresis,
          noiseGateAttack: noiseGateAttack,
          noiseGateHold: noiseGateHold,
          noiseGateRelease: noiseGateRelease,
          noiseGateBypass: noiseGateBypass,
          chorusRate: chorusRate,
          chorusDepth: chorusDepth,
          chorusMix: chorusMix,
          chorusBypass: chorusBypass,
          wahPosition: wahPosition,
          wahVocal: wahVocal,
          wahBypass: wahBypass,
        ),
      ),
    _ => const AsyncValue.loading(),
  };
}
