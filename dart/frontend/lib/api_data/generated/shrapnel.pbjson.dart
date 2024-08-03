//
//  Generated code. Do not modify.
//  source: shrapnel.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use messageDescriptor instead')
const Message$json = {
  '1': 'Message',
  '2': [
    {'1': 'audio_event', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.audio_events.Message', '9': 0, '10': 'audioEvent'},
    {'1': 'command', '3': 2, '4': 1, '5': 11, '6': '.shrapnel.cmd_handling.Message', '9': 0, '10': 'command'},
    {'1': 'midi_mapping', '3': 3, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.Message', '9': 0, '10': 'midiMapping'},
    {'1': 'preset', '3': 4, '4': 1, '5': 11, '6': '.shrapnel.presets.Message', '9': 0, '10': 'preset'},
    {'1': 'selected_preset', '3': 5, '4': 1, '5': 11, '6': '.shrapnel.selected_preset.Message', '9': 0, '10': 'selectedPreset'},
  ],
  '8': [
    {'1': 'message'},
  ],
};

/// Descriptor for `Message`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List messageDescriptor = $convert.base64Decode(
    'CgdNZXNzYWdlEkEKC2F1ZGlvX2V2ZW50GAEgASgLMh4uc2hyYXBuZWwuYXVkaW9fZXZlbnRzLk'
    '1lc3NhZ2VIAFIKYXVkaW9FdmVudBI6Cgdjb21tYW5kGAIgASgLMh4uc2hyYXBuZWwuY21kX2hh'
    'bmRsaW5nLk1lc3NhZ2VIAFIHY29tbWFuZBJDCgxtaWRpX21hcHBpbmcYAyABKAsyHi5zaHJhcG'
    '5lbC5taWRpX21hcHBpbmcuTWVzc2FnZUgAUgttaWRpTWFwcGluZxIzCgZwcmVzZXQYBCABKAsy'
    'GS5zaHJhcG5lbC5wcmVzZXRzLk1lc3NhZ2VIAFIGcHJlc2V0EkwKD3NlbGVjdGVkX3ByZXNldB'
    'gFIAEoCzIhLnNocmFwbmVsLnNlbGVjdGVkX3ByZXNldC5NZXNzYWdlSABSDnNlbGVjdGVkUHJl'
    'c2V0QgkKB21lc3NhZ2U=');
