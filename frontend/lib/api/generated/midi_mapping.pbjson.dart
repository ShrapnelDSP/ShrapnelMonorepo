//
//  Generated code. Do not modify.
//  source: midi_mapping.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:convert' as $convert;
import 'dart:core' as $core;
import 'dart:typed_data' as $typed_data;

@$core.Deprecated('Use mappingDescriptor instead')
const Mapping$json = {
  '1': 'Mapping',
  '2': [
    {'1': 'midi_channel', '3': 1, '4': 1, '5': 13, '10': 'midiChannel'},
    {'1': 'cc_number', '3': 2, '4': 1, '5': 13, '10': 'ccNumber'},
    {'1': 'mode', '3': 3, '4': 1, '5': 14, '6': '.shrapnel.midi_mapping.Mapping.Mode', '10': 'mode'},
    {'1': 'parameterName', '3': 4, '4': 1, '5': 9, '10': 'parameterName'},
  ],
  '4': [Mapping_Mode$json],
};

@$core.Deprecated('Use mappingDescriptor instead')
const Mapping_Mode$json = {
  '1': 'Mode',
  '2': [
    {'1': 'parameter', '2': 0},
    {'1': 'toggle', '2': 1},
  ],
};

/// Descriptor for `Mapping`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List mappingDescriptor = $convert.base64Decode(
    'CgdNYXBwaW5nEiEKDG1pZGlfY2hhbm5lbBgBIAEoDVILbWlkaUNoYW5uZWwSGwoJY2NfbnVtYm'
    'VyGAIgASgNUghjY051bWJlchI3CgRtb2RlGAMgASgOMiMuc2hyYXBuZWwubWlkaV9tYXBwaW5n'
    'Lk1hcHBpbmcuTW9kZVIEbW9kZRIkCg1wYXJhbWV0ZXJOYW1lGAQgASgJUg1wYXJhbWV0ZXJOYW'
    '1lIiEKBE1vZGUSDQoJcGFyYW1ldGVyEAASCgoGdG9nZ2xlEAE=');

@$core.Deprecated('Use midiMessageDescriptor instead')
const MidiMessage$json = {
  '1': 'MidiMessage',
  '2': [
    {'1': 'channel', '3': 1, '4': 1, '5': 13, '10': 'channel'},
    {'1': 'note_on', '3': 2, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MidiMessage.NoteOn', '9': 0, '10': 'noteOn'},
    {'1': 'note_off', '3': 3, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MidiMessage.NoteOff', '9': 0, '10': 'noteOff'},
    {'1': 'control_change', '3': 4, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MidiMessage.ControlChange', '9': 0, '10': 'controlChange'},
    {'1': 'program_change', '3': 5, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MidiMessage.ProgramChange', '9': 0, '10': 'programChange'},
  ],
  '3': [MidiMessage_NoteOn$json, MidiMessage_NoteOff$json, MidiMessage_ControlChange$json, MidiMessage_ProgramChange$json],
  '8': [
    {'1': 'parameters'},
  ],
};

@$core.Deprecated('Use midiMessageDescriptor instead')
const MidiMessage_NoteOn$json = {
  '1': 'NoteOn',
  '2': [
    {'1': 'note', '3': 1, '4': 1, '5': 13, '10': 'note'},
    {'1': 'velocity', '3': 2, '4': 1, '5': 13, '10': 'velocity'},
  ],
};

@$core.Deprecated('Use midiMessageDescriptor instead')
const MidiMessage_NoteOff$json = {
  '1': 'NoteOff',
  '2': [
    {'1': 'note', '3': 1, '4': 1, '5': 13, '10': 'note'},
    {'1': 'velocity', '3': 2, '4': 1, '5': 13, '10': 'velocity'},
  ],
};

@$core.Deprecated('Use midiMessageDescriptor instead')
const MidiMessage_ControlChange$json = {
  '1': 'ControlChange',
  '2': [
    {'1': 'control', '3': 1, '4': 1, '5': 13, '10': 'control'},
    {'1': 'value', '3': 2, '4': 1, '5': 13, '10': 'value'},
  ],
};

@$core.Deprecated('Use midiMessageDescriptor instead')
const MidiMessage_ProgramChange$json = {
  '1': 'ProgramChange',
  '2': [
    {'1': 'number', '3': 1, '4': 1, '5': 13, '10': 'number'},
  ],
};

/// Descriptor for `MidiMessage`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List midiMessageDescriptor = $convert.base64Decode(
    'CgtNaWRpTWVzc2FnZRIYCgdjaGFubmVsGAEgASgNUgdjaGFubmVsEkQKB25vdGVfb24YAiABKA'
    'syKS5zaHJhcG5lbC5taWRpX21hcHBpbmcuTWlkaU1lc3NhZ2UuTm90ZU9uSABSBm5vdGVPbhJH'
    'Cghub3RlX29mZhgDIAEoCzIqLnNocmFwbmVsLm1pZGlfbWFwcGluZy5NaWRpTWVzc2FnZS5Ob3'
    'RlT2ZmSABSB25vdGVPZmYSWQoOY29udHJvbF9jaGFuZ2UYBCABKAsyMC5zaHJhcG5lbC5taWRp'
    'X21hcHBpbmcuTWlkaU1lc3NhZ2UuQ29udHJvbENoYW5nZUgAUg1jb250cm9sQ2hhbmdlElkKDn'
    'Byb2dyYW1fY2hhbmdlGAUgASgLMjAuc2hyYXBuZWwubWlkaV9tYXBwaW5nLk1pZGlNZXNzYWdl'
    'LlByb2dyYW1DaGFuZ2VIAFINcHJvZ3JhbUNoYW5nZRo4CgZOb3RlT24SEgoEbm90ZRgBIAEoDV'
    'IEbm90ZRIaCgh2ZWxvY2l0eRgCIAEoDVIIdmVsb2NpdHkaOQoHTm90ZU9mZhISCgRub3RlGAEg'
    'ASgNUgRub3RlEhoKCHZlbG9jaXR5GAIgASgNUgh2ZWxvY2l0eRo/Cg1Db250cm9sQ2hhbmdlEh'
    'gKB2NvbnRyb2wYASABKA1SB2NvbnRyb2wSFAoFdmFsdWUYAiABKA1SBXZhbHVlGicKDVByb2dy'
    'YW1DaGFuZ2USFgoGbnVtYmVyGAEgASgNUgZudW1iZXJCDAoKcGFyYW1ldGVycw==');

@$core.Deprecated('Use mappingRecordDescriptor instead')
const MappingRecord$json = {
  '1': 'MappingRecord',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.uuid.Uuid', '10': 'id'},
    {'1': 'mapping', '3': 2, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.Mapping', '10': 'mapping'},
  ],
};

/// Descriptor for `MappingRecord`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List mappingRecordDescriptor = $convert.base64Decode(
    'Cg1NYXBwaW5nUmVjb3JkEiMKAmlkGAEgASgLMhMuc2hyYXBuZWwudXVpZC5VdWlkUgJpZBI4Cg'
    'dtYXBwaW5nGAIgASgLMh4uc2hyYXBuZWwubWlkaV9tYXBwaW5nLk1hcHBpbmdSB21hcHBpbmc=');

@$core.Deprecated('Use mappingListDescriptor instead')
const MappingList$json = {
  '1': 'MappingList',
  '2': [
    {'1': 'mappings', '3': 1, '4': 3, '5': 11, '6': '.shrapnel.midi_mapping.MappingRecord', '10': 'mappings'},
  ],
};

/// Descriptor for `MappingList`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List mappingListDescriptor = $convert.base64Decode(
    'CgtNYXBwaW5nTGlzdBJACghtYXBwaW5ncxgBIAMoCzIkLnNocmFwbmVsLm1pZGlfbWFwcGluZy'
    '5NYXBwaW5nUmVjb3JkUghtYXBwaW5ncw==');

@$core.Deprecated('Use getRequestDescriptor instead')
const GetRequest$json = {
  '1': 'GetRequest',
};

/// Descriptor for `GetRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List getRequestDescriptor = $convert.base64Decode(
    'CgpHZXRSZXF1ZXN0');

@$core.Deprecated('Use getResponseDescriptor instead')
const GetResponse$json = {
  '1': 'GetResponse',
  '2': [
    {'1': 'mappings', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MappingList', '10': 'mappings'},
  ],
};

/// Descriptor for `GetResponse`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List getResponseDescriptor = $convert.base64Decode(
    'CgtHZXRSZXNwb25zZRI+CghtYXBwaW5ncxgBIAEoCzIiLnNocmFwbmVsLm1pZGlfbWFwcGluZy'
    '5NYXBwaW5nTGlzdFIIbWFwcGluZ3M=');

@$core.Deprecated('Use createRequestDescriptor instead')
const CreateRequest$json = {
  '1': 'CreateRequest',
  '2': [
    {'1': 'mapping', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MappingRecord', '10': 'mapping'},
  ],
};

/// Descriptor for `CreateRequest`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List createRequestDescriptor = $convert.base64Decode(
    'Cg1DcmVhdGVSZXF1ZXN0Ej4KB21hcHBpbmcYASABKAsyJC5zaHJhcG5lbC5taWRpX21hcHBpbm'
    'cuTWFwcGluZ1JlY29yZFIHbWFwcGluZw==');

@$core.Deprecated('Use createResponseDescriptor instead')
const CreateResponse$json = {
  '1': 'CreateResponse',
  '2': [
    {'1': 'mapping', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MappingRecord', '10': 'mapping'},
  ],
};

/// Descriptor for `CreateResponse`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List createResponseDescriptor = $convert.base64Decode(
    'Cg5DcmVhdGVSZXNwb25zZRI+CgdtYXBwaW5nGAEgASgLMiQuc2hyYXBuZWwubWlkaV9tYXBwaW'
    '5nLk1hcHBpbmdSZWNvcmRSB21hcHBpbmc=');

@$core.Deprecated('Use updateDescriptor instead')
const Update$json = {
  '1': 'Update',
  '2': [
    {'1': 'mapping', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MappingRecord', '10': 'mapping'},
  ],
};

/// Descriptor for `Update`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List updateDescriptor = $convert.base64Decode(
    'CgZVcGRhdGUSPgoHbWFwcGluZxgBIAEoCzIkLnNocmFwbmVsLm1pZGlfbWFwcGluZy5NYXBwaW'
    '5nUmVjb3JkUgdtYXBwaW5n');

@$core.Deprecated('Use removeDescriptor instead')
const Remove$json = {
  '1': 'Remove',
  '2': [
    {'1': 'id', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.uuid.Uuid', '10': 'id'},
  ],
};

/// Descriptor for `Remove`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List removeDescriptor = $convert.base64Decode(
    'CgZSZW1vdmUSIwoCaWQYASABKAsyEy5zaHJhcG5lbC51dWlkLlV1aWRSAmlk');

@$core.Deprecated('Use messageReceivedDescriptor instead')
const MessageReceived$json = {
  '1': 'MessageReceived',
  '2': [
    {'1': 'received_message', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MidiMessage', '10': 'receivedMessage'},
  ],
};

/// Descriptor for `MessageReceived`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List messageReceivedDescriptor = $convert.base64Decode(
    'Cg9NZXNzYWdlUmVjZWl2ZWQSTQoQcmVjZWl2ZWRfbWVzc2FnZRgBIAEoCzIiLnNocmFwbmVsLm'
    '1pZGlfbWFwcGluZy5NaWRpTWVzc2FnZVIPcmVjZWl2ZWRNZXNzYWdl');

@$core.Deprecated('Use messageDescriptor instead')
const Message$json = {
  '1': 'Message',
  '2': [
    {'1': 'get_request', '3': 1, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.GetRequest', '9': 0, '10': 'getRequest'},
    {'1': 'get_response', '3': 2, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.GetResponse', '9': 0, '10': 'getResponse'},
    {'1': 'create_request', '3': 3, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.CreateRequest', '9': 0, '10': 'createRequest'},
    {'1': 'create_response', '3': 4, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.CreateResponse', '9': 0, '10': 'createResponse'},
    {'1': 'update', '3': 5, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.Update', '9': 0, '10': 'update'},
    {'1': 'remove', '3': 6, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.Remove', '9': 0, '10': 'remove'},
    {'1': 'message_received', '3': 7, '4': 1, '5': 11, '6': '.shrapnel.midi_mapping.MessageReceived', '9': 0, '10': 'messageReceived'},
  ],
  '8': [
    {'1': 'message'},
  ],
};

/// Descriptor for `Message`. Decode as a `google.protobuf.DescriptorProto`.
final $typed_data.Uint8List messageDescriptor = $convert.base64Decode(
    'CgdNZXNzYWdlEkQKC2dldF9yZXF1ZXN0GAEgASgLMiEuc2hyYXBuZWwubWlkaV9tYXBwaW5nLk'
    'dldFJlcXVlc3RIAFIKZ2V0UmVxdWVzdBJHCgxnZXRfcmVzcG9uc2UYAiABKAsyIi5zaHJhcG5l'
    'bC5taWRpX21hcHBpbmcuR2V0UmVzcG9uc2VIAFILZ2V0UmVzcG9uc2USTQoOY3JlYXRlX3JlcX'
    'Vlc3QYAyABKAsyJC5zaHJhcG5lbC5taWRpX21hcHBpbmcuQ3JlYXRlUmVxdWVzdEgAUg1jcmVh'
    'dGVSZXF1ZXN0ElAKD2NyZWF0ZV9yZXNwb25zZRgEIAEoCzIlLnNocmFwbmVsLm1pZGlfbWFwcG'
    'luZy5DcmVhdGVSZXNwb25zZUgAUg5jcmVhdGVSZXNwb25zZRI3CgZ1cGRhdGUYBSABKAsyHS5z'
    'aHJhcG5lbC5taWRpX21hcHBpbmcuVXBkYXRlSABSBnVwZGF0ZRI3CgZyZW1vdmUYBiABKAsyHS'
    '5zaHJhcG5lbC5taWRpX21hcHBpbmcuUmVtb3ZlSABSBnJlbW92ZRJTChBtZXNzYWdlX3JlY2Vp'
    'dmVkGAcgASgLMiYuc2hyYXBuZWwubWlkaV9tYXBwaW5nLk1lc3NhZ2VSZWNlaXZlZEgAUg9tZX'
    'NzYWdlUmVjZWl2ZWRCCQoHbWVzc2FnZQ==');

