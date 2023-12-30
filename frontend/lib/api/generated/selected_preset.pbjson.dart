//
//  Generated code. Do not modify.
//  source: selected_preset.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use readDescriptor instead')
const Read$json = {
  '1': 'Read',
};

/// Descriptor for `Read`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List readDescriptor = $convert.base64Decode('CgRSZWFk');

@$core.Deprecated('Use notifyDescriptor instead')
const Notify$json = {
  '1': 'Notify',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 13, '10': 'id'},
  ],
};

/// Descriptor for `Notify`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List notifyDescriptor =
    $convert.base64Decode('CgZOb3RpZnkSDgoCaWQYASABKA1SAmlk');

@$core.Deprecated('Use writeDescriptor instead')
const Write$json = {
  '1': 'Write',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 13, '10': 'id'},
  ],
};

/// Descriptor for `Write`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List writeDescriptor =
    $convert.base64Decode('CgVXcml0ZRIOCgJpZBgBIAEoDVICaWQ=');

@$core.Deprecated('Use messageDescriptor instead')
const Message$json = {
  '1': 'Message',
  '2': [
    {
      '1': 'read',
      '3': 1,
      '4': 1,
      '5': 11,
      '6': '.shrapnel.selected_preset.Read',
      '9': 0,
      '10': 'read'
    },
    {
      '1': 'notify',
      '3': 2,
      '4': 1,
      '5': 11,
      '6': '.shrapnel.selected_preset.Notify',
      '9': 0,
      '10': 'notify'
    },
    {
      '1': 'write',
      '3': 3,
      '4': 1,
      '5': 11,
      '6': '.shrapnel.selected_preset.Write',
      '9': 0,
      '10': 'write'
    },
  ],
  '8': [
    {'1': 'message'},
  ],
};

/// Descriptor for `Message`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List messageDescriptor = $convert.base64Decode(
    'CgdNZXNzYWdlEjQKBHJlYWQYASABKAsyHi5zaHJhcG5lbC5zZWxlY3RlZF9wcmVzZXQuUmVhZE'
    'gAUgRyZWFkEjoKBm5vdGlmeRgCIAEoCzIgLnNocmFwbmVsLnNlbGVjdGVkX3ByZXNldC5Ob3Rp'
    'ZnlIAFIGbm90aWZ5EjcKBXdyaXRlGAMgASgLMh8uc2hyYXBuZWwuc2VsZWN0ZWRfcHJlc2V0Ll'
    'dyaXRlSABSBXdyaXRlQgkKB21lc3NhZ2U=');
