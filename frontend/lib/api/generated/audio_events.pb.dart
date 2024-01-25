//
//  Generated code. Do not modify.
//  source: audio_events.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

/// Sent when the input level is too high, causing clipping
class InputClipped extends $pb.GeneratedMessage {
  factory InputClipped() => create();
  InputClipped._() : super();
  factory InputClipped.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory InputClipped.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'InputClipped', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.audio_events'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  InputClipped clone() => InputClipped()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  InputClipped copyWith(void Function(InputClipped) updates) => super.copyWith((message) => updates(message as InputClipped)) as InputClipped;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static InputClipped create() => InputClipped._();
  InputClipped createEmptyInstance() => create();
  static $pb.PbList<InputClipped> createRepeated() => $pb.PbList<InputClipped>();
  @$core.pragma('dart2js:noInline')
  static InputClipped getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<InputClipped>(create);
  static InputClipped? _defaultInstance;
}

/// Sent when the output level is too high, causing clipping
class OutputClipped extends $pb.GeneratedMessage {
  factory OutputClipped() => create();
  OutputClipped._() : super();
  factory OutputClipped.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory OutputClipped.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'OutputClipped', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.audio_events'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  OutputClipped clone() => OutputClipped()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  OutputClipped copyWith(void Function(OutputClipped) updates) => super.copyWith((message) => updates(message as OutputClipped)) as OutputClipped;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static OutputClipped create() => OutputClipped._();
  OutputClipped createEmptyInstance() => create();
  static $pb.PbList<OutputClipped> createRepeated() => $pb.PbList<OutputClipped>();
  @$core.pragma('dart2js:noInline')
  static OutputClipped getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<OutputClipped>(create);
  static OutputClipped? _defaultInstance;
}

enum Message_Event {
  inputClipped, 
  outputClipped, 
  notSet
}

class Message extends $pb.GeneratedMessage {
  factory Message({
    InputClipped? inputClipped,
    OutputClipped? outputClipped,
  }) {
    final $result = create();
    if (inputClipped != null) {
      $result.inputClipped = inputClipped;
    }
    if (outputClipped != null) {
      $result.outputClipped = outputClipped;
    }
    return $result;
  }
  Message._() : super();
  factory Message.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Message.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, Message_Event> _Message_EventByTag = {
    1 : Message_Event.inputClipped,
    2 : Message_Event.outputClipped,
    0 : Message_Event.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Message', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.audio_events'), createEmptyInstance: create)
    ..oo(0, [1, 2])
    ..aOM<InputClipped>(1, _omitFieldNames ? '' : 'inputClipped', subBuilder: InputClipped.create)
    ..aOM<OutputClipped>(2, _omitFieldNames ? '' : 'outputClipped', subBuilder: OutputClipped.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Message clone() => Message()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Message copyWith(void Function(Message) updates) => super.copyWith((message) => updates(message as Message)) as Message;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Message create() => Message._();
  Message createEmptyInstance() => create();
  static $pb.PbList<Message> createRepeated() => $pb.PbList<Message>();
  @$core.pragma('dart2js:noInline')
  static Message getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Message>(create);
  static Message? _defaultInstance;

  Message_Event whichEvent() => _Message_EventByTag[$_whichOneof(0)]!;
  void clearEvent() => clearField($_whichOneof(0));

  @$pb.TagNumber(1)
  InputClipped get inputClipped => $_getN(0);
  @$pb.TagNumber(1)
  set inputClipped(InputClipped v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasInputClipped() => $_has(0);
  @$pb.TagNumber(1)
  void clearInputClipped() => clearField(1);
  @$pb.TagNumber(1)
  InputClipped ensureInputClipped() => $_ensure(0);

  @$pb.TagNumber(2)
  OutputClipped get outputClipped => $_getN(1);
  @$pb.TagNumber(2)
  set outputClipped(OutputClipped v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasOutputClipped() => $_has(1);
  @$pb.TagNumber(2)
  void clearOutputClipped() => clearField(2);
  @$pb.TagNumber(2)
  OutputClipped ensureOutputClipped() => $_ensure(1);
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
