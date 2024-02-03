import 'package:riverpod_annotation/riverpod_annotation.dart';

import '../../parameter.dart';
import 'presets.dart';

part 'current_parameters.g.dart';

@riverpod
AsyncValue<PresetParametersData> currentParameters(CurrentParametersRef ref) {
  final ampGain = ref.read(audioParameterDoubleModelProvider('ampGain')).value;
  final ampChannel =
      ref.read(audioParameterDoubleModelProvider('ampChannel')).value;
  final bass = ref.read(audioParameterDoubleModelProvider('bass')).value;
  final middle = ref.read(audioParameterDoubleModelProvider('middle')).value;
  final treble = ref.read(audioParameterDoubleModelProvider('treble')).value;
  final contour = ref.read(audioParameterDoubleModelProvider('contour')).value;
  final volume = ref.read(audioParameterDoubleModelProvider('volume')).value;
  final noiseGateThreshold =
      ref.read(audioParameterDoubleModelProvider('noiseGateThreshold')).value;
  final noiseGateHysteresis =
      ref.read(audioParameterDoubleModelProvider('noiseGateHysteresis')).value;
  final noiseGateAttack =
      ref.read(audioParameterDoubleModelProvider('noiseGateAttack')).value;
  final noiseGateHold =
      ref.read(audioParameterDoubleModelProvider('noiseGateHold')).value;
  final noiseGateRelease =
      ref.read(audioParameterDoubleModelProvider('noiseGateRelease')).value;
  final noiseGateBypass =
      ref.read(audioParameterDoubleModelProvider('noiseGateBypass')).value;
  final chorusRate =
      ref.read(audioParameterDoubleModelProvider('chorusRate')).value;
  final chorusDepth =
      ref.read(audioParameterDoubleModelProvider('chorusDepth')).value;
  final chorusMix =
      ref.read(audioParameterDoubleModelProvider('chorusMix')).value;
  final chorusBypass =
      ref.read(audioParameterDoubleModelProvider('chorusBypass')).value;
  final wahPosition =
      ref.read(audioParameterDoubleModelProvider('wahPosition')).value;
  final wahVocal =
      ref.read(audioParameterDoubleModelProvider('wahVocal')).value;
  final wahBypass =
      ref.read(audioParameterDoubleModelProvider('wahBypass')).value;

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
