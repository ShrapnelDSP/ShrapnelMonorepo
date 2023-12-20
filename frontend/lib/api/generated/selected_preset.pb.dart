//
//  Generated code. Do not modify.
//  source: selected_preset.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

class Read extends $pb.GeneratedMessage {
  factory Read() => create();
  Read._() : super();
  factory Read.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Read.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Read', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.selected_preset'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Read clone() => Read()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Read copyWith(void Function(Read) updates) => super.copyWith((message) => updates(message as Read)) as Read;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Read create() => Read._();
  Read createEmptyInstance() => create();
  static $pb.PbList<Read> createRepeated() => $pb.PbList<Read>();
  @$core.pragma('dart2js:noInline')
  static Read getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Read>(create);
  static Read? _defaultInstance;
}

class Notify extends $pb.GeneratedMessage {
  factory Notify({
    $core.int? id,
  }) {
    final $result = create();
    if (id != null) {
      $result.id = id;
    }
    return $result;
  }
  Notify._() : super();
  factory Notify.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Notify.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Notify', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.selected_preset'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'id', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Notify clone() => Notify()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Notify copyWith(void Function(Notify) updates) => super.copyWith((message) => updates(message as Notify)) as Notify;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Notify create() => Notify._();
  Notify createEmptyInstance() => create();
  static $pb.PbList<Notify> createRepeated() => $pb.PbList<Notify>();
  @$core.pragma('dart2js:noInline')
  static Notify getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Notify>(create);
  static Notify? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => clearField(1);
}

class Write extends $pb.GeneratedMessage {
  factory Write({
    $core.int? id,
  }) {
    final $result = create();
    if (id != null) {
      $result.id = id;
    }
    return $result;
  }
  Write._() : super();
  factory Write.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Write.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Write', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.selected_preset'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'id', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Write clone() => Write()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Write copyWith(void Function(Write) updates) => super.copyWith((message) => updates(message as Write)) as Write;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Write create() => Write._();
  Write createEmptyInstance() => create();
  static $pb.PbList<Write> createRepeated() => $pb.PbList<Write>();
  @$core.pragma('dart2js:noInline')
  static Write getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Write>(create);
  static Write? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => clearField(1);
}

enum Message_Message {
  read, 
  notify, 
  write, 
  notSet
}

class Message extends $pb.GeneratedMessage {
  factory Message({
    Read? read,
    Notify? notify,
    Write? write,
  }) {
    final $result = create();
    if (read != null) {
      $result.read = read;
    }
    if (notify != null) {
      $result.notify = notify;
    }
    if (write != null) {
      $result.write = write;
    }
    return $result;
  }
  Message._() : super();
  factory Message.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Message.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, Message_Message> _Message_MessageByTag = {
    1 : Message_Message.read,
    2 : Message_Message.notify,
    3 : Message_Message.write,
    0 : Message_Message.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Message', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.selected_preset'), createEmptyInstance: create)
    ..oo(0, [1, 2, 3])
    ..aOM<Read>(1, _omitFieldNames ? '' : 'read', subBuilder: Read.create)
    ..aOM<Notify>(2, _omitFieldNames ? '' : 'notify', subBuilder: Notify.create)
    ..aOM<Write>(3, _omitFieldNames ? '' : 'write', subBuilder: Write.create)
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

  Message_Message whichMessage() => _Message_MessageByTag[$_whichOneof(0)]!;
  void clearMessage() => clearField($_whichOneof(0));

  @$pb.TagNumber(1)
  Read get read => $_getN(0);
  @$pb.TagNumber(1)
  set read(Read v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasRead() => $_has(0);
  @$pb.TagNumber(1)
  void clearRead() => clearField(1);
  @$pb.TagNumber(1)
  Read ensureRead() => $_ensure(0);

  @$pb.TagNumber(2)
  Notify get notify => $_getN(1);
  @$pb.TagNumber(2)
  set notify(Notify v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasNotify() => $_has(1);
  @$pb.TagNumber(2)
  void clearNotify() => clearField(2);
  @$pb.TagNumber(2)
  Notify ensureNotify() => $_ensure(1);

  @$pb.TagNumber(3)
  Write get write => $_getN(2);
  @$pb.TagNumber(3)
  set write(Write v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasWrite() => $_has(2);
  @$pb.TagNumber(3)
  void clearWrite() => clearField(3);
  @$pb.TagNumber(3)
  Write ensureWrite() => $_ensure(2);
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
