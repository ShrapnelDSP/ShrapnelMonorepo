import 'package:riverpod_annotation/riverpod_annotation.dart';

import 'parameter.dart';

part 'parameters_meta.g.dart';

const _parameters = <String, AudioParameterMetaData>{
  'chorusDepth': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'DEPTH',
    id: 'chorusDepth',
  ),
  'chorusMix': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'MIX',
    id: 'chorusMix',
  ),
  'chorusRate': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'RATE',
    id: 'chorusRate',
  ),
  'chorusBypass': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'Bypass',
    id: 'chorusBypass',
  ),
  'heavyMetalLevel': AudioParameterMetaData(
    groupName: 'Heavy Metal',
    name: 'LEVEL',
    id: 'heavyMetalLevel',
  ),
  'heavyMetalLow': AudioParameterMetaData(
    groupName: 'Heavy Metal',
    name: 'LOW',
    id: 'heavyMetalLow',
  ),
  'heavyMetalHigh': AudioParameterMetaData(
    groupName: 'Heavy Metal',
    name: 'HIGH',
    id: 'heavyMetalHigh',
  ),
  'heavyMetalDistortion': AudioParameterMetaData(
    groupName: 'Heavy Metal',
    name: 'DISTORTION',
    id: 'heavyMetalDistortion',
  ),
  'noiseGateAttack': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Attack',
    id: 'noiseGateAttack',
  ),
  'noiseGateHold': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Hold',
    id: 'noiseGateHold',
  ),
  'noiseGateHysteresis': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Hysteresis',
    id: 'noiseGateHysteresis',
  ),
  'noiseGateRelease': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Release',
    id: 'noiseGateRelease',
  ),
  'noiseGateThreshold': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Threshold',
    id: 'noiseGateThreshold',
  ),
  'noiseGateBypass': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Bypass',
    id: 'noiseGateBypass',
  ),
  'tubeScreamerDrive': AudioParameterMetaData(
    groupName: 'Tube Screamer',
    name: 'DRIVE',
    id: 'tubeScreamerDrive',
  ),
  'tubeScreamerTone': AudioParameterMetaData(
    groupName: 'Tube Screamer',
    name: 'TONE',
    id: 'tubeScreamerTone',
  ),
  'tubeScreamerLevel': AudioParameterMetaData(
    groupName: 'Tube Screamer',
    name: 'LEVEL',
    id: 'tubeScreamerLevel',
  ),
  'tubeScreamerBypass': AudioParameterMetaData(
    groupName: 'Tube Screamer',
    name: 'Bypass',
    id: 'tubeScreamerBypass',
  ),
  'wahPosition': AudioParameterMetaData(
    groupName: 'Wah',
    name: 'Position',
    id: 'wahPosition',
  ),
  'wahVocal': AudioParameterMetaData(
    groupName: 'Wah',
    name: 'Vocal',
    id: 'wahVocal',
  ),
  'wahBypass': AudioParameterMetaData(
    groupName: 'Wah',
    name: 'Bypass',
    id: 'wahBypass',
  ),
  'ampChannel': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'OD1/OD2',
    id: 'ampChannel',
  ),
  'ampGain': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Gain',
    id: 'ampGain',
  ),
  'bass': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Bass',
    id: 'bass',
  ),
  'middle': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Middle',
    id: 'middle',
  ),
  'treble': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Treble',
    id: 'treble',
  ),
  'contour': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Contour',
    id: 'contour',
  ),
  'volume': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Volume',
    id: 'volume',
  ),
};

@riverpod
AudioParameterMetaData audioParameterMetadata(
  AudioParameterMetadataRef ref,
  String parameterId,
) =>
    ref.watch(allParametersProvider)[parameterId]!;

@riverpod
Map<String, AudioParameterMetaData> allParameters(AllParametersRef ref) =>
    _parameters;

@riverpod
Map<String, String> parameterNames(ParameterNamesRef ref) =>
    ref.watch(allParametersProvider).map(
          (id, param) => MapEntry(id, '${param.groupName}: ${param.name}'),
        );
