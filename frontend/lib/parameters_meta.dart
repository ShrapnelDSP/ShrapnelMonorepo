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

import 'parameter.dart';

part 'parameters_meta.g.dart';

const _parameters = <String, AudioParameterMetaData>{
  'chorusDepth': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'DEPTH',
    id: 'chorusDepth',
    min: 0,
    max: 1,
    defaultValue: 0.3,
  ),
  'chorusMix': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'MIX',
    id: 'chorusMix',
    min: 0,
    max: 1,
    defaultValue: 0.8,
  ),
  'chorusRate': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'RATE',
    id: 'chorusRate',
    min: 0.1,
    max: 4,
    defaultValue: 0.95,
  ),
  'chorusBypass': AudioParameterMetaData(
    groupName: 'Chorus',
    name: 'Bypass',
    id: 'chorusBypass',
    min: 0,
    max: 1,
    defaultValue: 1,
  ),
  'noiseGateAttack': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Attack',
    id: 'noiseGateAttack',
    min: 1,
    max: 50,
    defaultValue: 10,
  ),
  'noiseGateHold': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Hold',
    id: 'noiseGateHold',
    min: 1,
    max: 250,
    defaultValue: 50,
  ),
  'noiseGateHysteresis': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Hysteresis',
    id: 'noiseGateHysteresis',
    min: 0,
    max: 5,
    defaultValue: 0,
  ),
  'noiseGateRelease': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Release',
    id: 'noiseGateRelease',
    min: 1,
    max: 250,
    defaultValue: 50,
  ),
  'noiseGateThreshold': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Threshold',
    id: 'noiseGateThreshold',
    min: -80,
    max: 0,
    defaultValue: -60,
  ),
  'noiseGateBypass': AudioParameterMetaData(
    groupName: 'Noise Gate',
    name: 'Bypass',
    id: 'noiseGateBypass',
    min: 0,
    max: 1,
    defaultValue: 1,
  ),
  'wahPosition': AudioParameterMetaData(
    groupName: 'Wah',
    name: 'Position',
    id: 'wahPosition',
    min: 0,
    max: 1,
    defaultValue: 0.5,
  ),
  'wahVocal': AudioParameterMetaData(
    groupName: 'Wah',
    name: 'Vocal',
    id: 'wahVocal',
    min: 0,
    max: 1,
    defaultValue: 0,
  ),
  'wahBypass': AudioParameterMetaData(
    groupName: 'Wah',
    name: 'Bypass',
    id: 'wahBypass',
    min: 0,
    max: 1,
    defaultValue: 1,
  ),
  'ampChannel': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'OD1/OD2',
    id: 'ampChannel',
    min: 0,
    max: 1,
    defaultValue: 0,
  ),
  'ampGain': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Gain',
    id: 'ampGain',
    min: 0,
    max: 1,
    defaultValue: 0.5,
  ),
  'bass': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Bass',
    id: 'bass',
    min: 0,
    max: 1,
    defaultValue: 0.5,
  ),
  'middle': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Middle',
    id: 'middle',
    min: 0,
    max: 1,
    defaultValue: 0.5,
  ),
  'treble': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Treble',
    id: 'treble',
    min: 0,
    max: 1,
    defaultValue: 0.5,
  ),
  'contour': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Contour',
    id: 'contour',
    min: 0.01,
    max: 1,
    defaultValue: 0.5,
  ),
  'volume': AudioParameterMetaData(
    groupName: 'Valvestate',
    name: 'Volume',
    id: 'volume',
    min: -30,
    max: 0,
    defaultValue: -15,
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
