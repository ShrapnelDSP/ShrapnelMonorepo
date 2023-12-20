//
//  Generated code. Do not modify.
//  source: midi_mapping.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

class Mapping_Mode extends $pb.ProtobufEnum {
  static const Mapping_Mode parameter = Mapping_Mode._(0, _omitEnumNames ? '' : 'parameter');
  static const Mapping_Mode toggle = Mapping_Mode._(1, _omitEnumNames ? '' : 'toggle');

  static const $core.List<Mapping_Mode> values = <Mapping_Mode> [
    parameter,
    toggle,
  ];

  static final $core.Map<$core.int, Mapping_Mode> _byValue = $pb.ProtobufEnum.initByValue(values);
  static Mapping_Mode? valueOf($core.int value) => _byValue[value];

  const Mapping_Mode._($core.int v, $core.String n) : super(v, n);
}


const _omitEnumNames = $core.bool.fromEnvironment('protobuf.omit_enum_names');
