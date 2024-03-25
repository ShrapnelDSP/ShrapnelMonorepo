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
  final ampGain = ref.watch(audioParameterDoubleModelProvider('ampGain')).value;
  final ampChannel =
      ref.watch(audioParameterDoubleModelProvider('ampChannel')).value;
  final bass = ref.watch(audioParameterDoubleModelProvider('bass')).value;
  final middle = ref.watch(audioParameterDoubleModelProvider('middle')).value;
  final treble = ref.watch(audioParameterDoubleModelProvider('treble')).value;
  final contour = ref.watch(audioParameterDoubleModelProvider('contour')).value;
  final volume = ref.watch(audioParameterDoubleModelProvider('volume')).value;
  final noiseGateThreshold =
      ref.watch(audioParameterDoubleModelProvider('noiseGateThreshold')).value;
  final noiseGateHysteresis =
      ref.watch(audioParameterDoubleModelProvider('noiseGateHysteresis')).value;
  final noiseGateAttack =
      ref.watch(audioParameterDoubleModelProvider('noiseGateAttack')).value;
  final noiseGateHold =
      ref.watch(audioParameterDoubleModelProvider('noiseGateHold')).value;
  final noiseGateRelease =
      ref.watch(audioParameterDoubleModelProvider('noiseGateRelease')).value;
  final noiseGateBypass =
      ref.watch(audioParameterDoubleModelProvider('noiseGateBypass')).value;
  final chorusRate =
      ref.watch(audioParameterDoubleModelProvider('chorusRate')).value;
  final chorusDepth =
      ref.watch(audioParameterDoubleModelProvider('chorusDepth')).value;
  final chorusMix =
      ref.watch(audioParameterDoubleModelProvider('chorusMix')).value;
  final chorusBypass =
      ref.watch(audioParameterDoubleModelProvider('chorusBypass')).value;
  final wahPosition =
      ref.watch(audioParameterDoubleModelProvider('wahPosition')).value;
  final wahVocal =
      ref.watch(audioParameterDoubleModelProvider('wahVocal')).value;
  final wahBypass =
      ref.watch(audioParameterDoubleModelProvider('wahBypass')).value;

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
