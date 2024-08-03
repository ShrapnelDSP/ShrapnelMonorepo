//
//  Generated code. Do not modify.
//  source: audio_events.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use inputClippedDescriptor instead')
const InputClipped$json = {
  '1': 'InputClipped',
};

/// Descriptor for `InputClipped`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List inputClippedDescriptor = $convert.base64Decode(
    'CgxJbnB1dENsaXBwZWQ=');

@$core.Deprecated('Use outputClippedDescriptor instead')
const OutputClipped$json = {
  '1': 'OutputClipped',
};

/// Descriptor for `OutputClipped`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List outputClippedDescriptor = $convert.base64Decode(
    'Cg1PdXRwdXRDbGlwcGVk');

@$core.Deprecated('Use messageDescriptor instead')
const Message$json = {
  '1': 'Message',
  '2': [
    {'1': 'input_clipped', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.audio_events.InputClipped', '9': 0, '10': 'inputClipped'},
    {'1': 'output_clipped', '3': 2, '4': 1, '5': 11, '6': '.shrapnel.audio_events.OutputClipped', '9': 0, '10': 'outputClipped'},
  ],
  '8': [
    {'1': 'event'},
  ],
};

/// Descriptor for `Message`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List messageDescriptor = $convert.base64Decode(
    'CgdNZXNzYWdlEkoKDWlucHV0X2NsaXBwZWQYASABKAsyIy5zaHJhcG5lbC5hdWRpb19ldmVudH'
    'MuSW5wdXRDbGlwcGVkSABSDGlucHV0Q2xpcHBlZBJNCg5vdXRwdXRfY2xpcHBlZBgCIAEoCzIk'
    'LnNocmFwbmVsLmF1ZGlvX2V2ZW50cy5PdXRwdXRDbGlwcGVkSABSDW91dHB1dENsaXBwZWRCBw'
    'oFZXZlbnQ=');

