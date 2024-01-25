//
//  Generated code. Do not modify.
//  source: cmd_handling.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use updateDescriptor instead')
const Update$json = {
  '1': 'Update',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 9, '10': 'id'},
    {'1': 'value', '3': 2, '4': 1, '5': 2, '10': 'value'},
  ],
};

/// Descriptor for `Update`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List updateDescriptor = $convert.base64Decode(
    'CgZVcGRhdGUSDgoCaWQYASABKAlSAmlkEhQKBXZhbHVlGAIgASgCUgV2YWx1ZQ==');

@$core.Deprecated('Use initialiseDescriptor instead')
const Initialise$json = {
  '1': 'Initialise',
};

/// Descriptor for `Initialise`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List initialiseDescriptor = $convert.base64Decode(
    'CgpJbml0aWFsaXNl');

@$core.Deprecated('Use messageDescriptor instead')
const Message$json = {
  '1': 'Message',
  '2': [
    {'1': 'update', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.cmd_handling.Update', '9': 0, '10': 'update'},
    {'1': 'initialise', '3': 2, '4': 1, '5': 11, '6': '.shrapnel.cmd_handling.Initialise', '9': 0, '10': 'initialise'},
  ],
  '8': [
    {'1': 'message'},
  ],
};

/// Descriptor for `Message`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List messageDescriptor = $convert.base64Decode(
    'CgdNZXNzYWdlEjcKBnVwZGF0ZRgBIAEoCzIdLnNocmFwbmVsLmNtZF9oYW5kbGluZy5VcGRhdG'
    'VIAFIGdXBkYXRlEkMKCmluaXRpYWxpc2UYAiABKAsyIS5zaHJhcG5lbC5jbWRfaGFuZGxpbmcu'
    'SW5pdGlhbGlzZUgAUgppbml0aWFsaXNlQgkKB21lc3NhZ2U=');

