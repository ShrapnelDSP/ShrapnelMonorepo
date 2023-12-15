//
//  Generated code. Do not modify.
//  source: presets.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use presetParametersDescriptor instead')
const PresetParameters$json = {
  '1': 'PresetParameters',
  '2': [
    {'1': 'amp_gain', '3': 1, '4': 1, '5': 5, '10': 'ampGain'},
    {'1': 'amp_channel', '3': 2, '4': 1, '5': 5, '10': 'ampChannel'},
    {'1': 'bass', '3': 3, '4': 1, '5': 5, '10': 'bass'},
    {'1': 'middle', '3': 4, '4': 1, '5': 5, '10': 'middle'},
    {'1': 'treble', '3': 5, '4': 1, '5': 5, '10': 'treble'},
    {'1': 'contour', '3': 6, '4': 1, '5': 5, '10': 'contour'},
    {'1': 'volume', '3': 7, '4': 1, '5': 5, '10': 'volume'},
    {
      '1': 'noise_gate_threshold',
      '3': 8,
      '4': 1,
      '5': 5,
      '10': 'noiseGateThreshold'
    },
    {
      '1': 'noise_gate_hysteresis',
      '3': 9,
      '4': 1,
      '5': 5,
      '10': 'noiseGateHysteresis'
    },
    {
      '1': 'noise_gate_attack',
      '3': 10,
      '4': 1,
      '5': 5,
      '10': 'noiseGateAttack'
    },
    {'1': 'noise_gate_hold', '3': 11, '4': 1, '5': 5, '10': 'noiseGateHold'},
    {
      '1': 'noise_gate_release',
      '3': 12,
      '4': 1,
      '5': 5,
      '10': 'noiseGateRelease'
    },
    {
      '1': 'noise_gate_bypass',
      '3': 13,
      '4': 1,
      '5': 5,
      '10': 'noiseGateBypass'
    },
    {'1': 'chorus_rate', '3': 14, '4': 1, '5': 5, '10': 'chorusRate'},
    {'1': 'chorus_depth', '3': 15, '4': 1, '5': 5, '10': 'chorusDepth'},
    {'1': 'chorus_mix', '3': 16, '4': 1, '5': 5, '10': 'chorusMix'},
    {'1': 'chorus_bypass', '3': 17, '4': 1, '5': 5, '10': 'chorusBypass'},
    {'1': 'wah_position', '3': 18, '4': 1, '5': 5, '10': 'wahPosition'},
    {'1': 'wah_vocal', '3': 19, '4': 1, '5': 5, '10': 'wahVocal'},
    {'1': 'wah_bypass', '3': 20, '4': 1, '5': 5, '10': 'wahBypass'},
  ],
};

/// Descriptor for `PresetParameters`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List presetParametersDescriptor = $convert.base64Decode(
    'ChBQcmVzZXRQYXJhbWV0ZXJzEhkKCGFtcF9nYWluGAEgASgFUgdhbXBHYWluEh8KC2FtcF9jaG'
    'FubmVsGAIgASgFUgphbXBDaGFubmVsEhIKBGJhc3MYAyABKAVSBGJhc3MSFgoGbWlkZGxlGAQg'
    'ASgFUgZtaWRkbGUSFgoGdHJlYmxlGAUgASgFUgZ0cmVibGUSGAoHY29udG91chgGIAEoBVIHY2'
    '9udG91chIWCgZ2b2x1bWUYByABKAVSBnZvbHVtZRIwChRub2lzZV9nYXRlX3RocmVzaG9sZBgI'
    'IAEoBVISbm9pc2VHYXRlVGhyZXNob2xkEjIKFW5vaXNlX2dhdGVfaHlzdGVyZXNpcxgJIAEoBV'
    'ITbm9pc2VHYXRlSHlzdGVyZXNpcxIqChFub2lzZV9nYXRlX2F0dGFjaxgKIAEoBVIPbm9pc2VH'
    'YXRlQXR0YWNrEiYKD25vaXNlX2dhdGVfaG9sZBgLIAEoBVINbm9pc2VHYXRlSG9sZBIsChJub2'
    'lzZV9nYXRlX3JlbGVhc2UYDCABKAVSEG5vaXNlR2F0ZVJlbGVhc2USKgoRbm9pc2VfZ2F0ZV9i'
    'eXBhc3MYDSABKAVSD25vaXNlR2F0ZUJ5cGFzcxIfCgtjaG9ydXNfcmF0ZRgOIAEoBVIKY2hvcn'
    'VzUmF0ZRIhCgxjaG9ydXNfZGVwdGgYDyABKAVSC2Nob3J1c0RlcHRoEh0KCmNob3J1c19taXgY'
    'ECABKAVSCWNob3J1c01peBIjCg1jaG9ydXNfYnlwYXNzGBEgASgFUgxjaG9ydXNCeXBhc3MSIQ'
    'oMd2FoX3Bvc2l0aW9uGBIgASgFUgt3YWhQb3NpdGlvbhIbCgl3YWhfdm9jYWwYEyABKAVSCHdh'
    'aFZvY2FsEh0KCndhaF9ieXBhc3MYFCABKAVSCXdhaEJ5cGFzcw==');

@$core.Deprecated('Use presetRecordDescriptor instead')
const PresetRecord$json = {
  '1': 'PresetRecord',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 13, '10': 'id'},
    {'1': 'preset', '3': 2, '4': 1, '5': 11, '6': '.Preset', '10': 'preset'},
  ],
};

/// Descriptor for `PresetRecord`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List presetRecordDescriptor = $convert.base64Decode(
    'CgxQcmVzZXRSZWNvcmQSDgoCaWQYASABKA1SAmlkEh8KBnByZXNldBgCIAEoCzIHLlByZXNldF'
    'IGcHJlc2V0');

@$core.Deprecated('Use presetDescriptor instead')
const Preset$json = {
  '1': 'Preset',
  '2': [
    {'1': 'name', '3': 1, '4': 1, '5': 9, '10': 'name'},
    {
      '1': 'parameters',
      '3': 2,
      '4': 1,
      '5': 11,
      '6': '.PresetParameters',
      '10': 'parameters'
    },
  ],
};

/// Descriptor for `Preset`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List presetDescriptor = $convert.base64Decode(
    'CgZQcmVzZXQSEgoEbmFtZRgBIAEoCVIEbmFtZRIxCgpwYXJhbWV0ZXJzGAIgASgLMhEuUHJlc2'
    'V0UGFyYW1ldGVyc1IKcGFyYW1ldGVycw==');
