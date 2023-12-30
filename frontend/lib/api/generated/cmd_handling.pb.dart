//
//  Generated code. Do not modify.
//  source: cmd_handling.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

class Update extends $pb.GeneratedMessage {
  factory Update({
    $core.String? id,
    $core.double? value,
  }) {
    final $result = create();
    if (id != null) {
      $result.id = id;
    }
    if (value != null) {
      $result.value = value;
    }
    return $result;
  }
  Update._() : super();
  factory Update.fromBuffer($core.List<$core.int> i,
          [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(i, r);
  factory Update.fromJson($core.String i,
          [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'Update',
      package: const $pb.PackageName(
          _omitMessageNames ? '' : 'shrapnel.cmd_handling'),
      createEmptyInstance: create)
    ..aOS(1, _omitFieldNames ? '' : 'id')
    ..a<$core.double>(2, _omitFieldNames ? '' : 'value', $pb.PbFieldType.OF)
    ..hasRequiredFields = false;

  @$core.Deprecated('Using this can add significant overhead to your binary. '
      'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
      'Will be removed in next major version')
  Update clone() => Update()..mergeFromMessage(this);
  @$core.Deprecated('Using this can add significant overhead to your binary. '
      'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
      'Will be removed in next major version')
  Update copyWith(void Function(Update) updates) =>
      super.copyWith((message) => updates(message as Update)) as Update;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Update create() => Update._();
  Update createEmptyInstance() => create();
  static $pb.PbList<Update> createRepeated() => $pb.PbList<Update>();
  @$core.pragma('dart2js:noInline')
  static Update getDefault() =>
      _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Update>(create);
  static Update? _defaultInstance;

  @$pb.TagNumber(1)
  $core.String get id => $_getSZ(0);
  @$pb.TagNumber(1)
  set id($core.String v) {
    $_setString(0, v);
  }

  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => clearField(1);

  @$pb.TagNumber(2)
  $core.double get value => $_getN(1);
  @$pb.TagNumber(2)
  set value($core.double v) {
    $_setFloat(1, v);
  }

  @$pb.TagNumber(2)
  $core.bool hasValue() => $_has(1);
  @$pb.TagNumber(2)
  void clearValue() => clearField(2);
}

class Initialise extends $pb.GeneratedMessage {
  factory Initialise() => create();
  Initialise._() : super();
  factory Initialise.fromBuffer($core.List<$core.int> i,
          [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(i, r);
  factory Initialise.fromJson($core.String i,
          [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'Initialise',
      package: const $pb.PackageName(
          _omitMessageNames ? '' : 'shrapnel.cmd_handling'),
      createEmptyInstance: create)
    ..hasRequiredFields = false;

  @$core.Deprecated('Using this can add significant overhead to your binary. '
      'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
      'Will be removed in next major version')
  Initialise clone() => Initialise()..mergeFromMessage(this);
  @$core.Deprecated('Using this can add significant overhead to your binary. '
      'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
      'Will be removed in next major version')
  Initialise copyWith(void Function(Initialise) updates) =>
      super.copyWith((message) => updates(message as Initialise)) as Initialise;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Initialise create() => Initialise._();
  Initialise createEmptyInstance() => create();
  static $pb.PbList<Initialise> createRepeated() => $pb.PbList<Initialise>();
  @$core.pragma('dart2js:noInline')
  static Initialise getDefault() => _defaultInstance ??=
      $pb.GeneratedMessage.$_defaultFor<Initialise>(create);
  static Initialise? _defaultInstance;
}

enum Message_Message { update, initialise, notSet }

class Message extends $pb.GeneratedMessage {
  factory Message({
    Update? update,
    Initialise? initialise,
  }) {
    final $result = create();
    if (update != null) {
      $result.update = update;
    }
    if (initialise != null) {
      $result.initialise = initialise;
    }
    return $result;
  }
  Message._() : super();
  factory Message.fromBuffer($core.List<$core.int> i,
          [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromBuffer(i, r);
  factory Message.fromJson($core.String i,
          [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) =>
      create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, Message_Message> _Message_MessageByTag = {
    1: Message_Message.update,
    2: Message_Message.initialise,
    0: Message_Message.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(
      _omitMessageNames ? '' : 'Message',
      package: const $pb.PackageName(
          _omitMessageNames ? '' : 'shrapnel.cmd_handling'),
      createEmptyInstance: create)
    ..oo(0, [1, 2])
    ..aOM<Update>(1, _omitFieldNames ? '' : 'update', subBuilder: Update.create)
    ..aOM<Initialise>(2, _omitFieldNames ? '' : 'initialise',
        subBuilder: Initialise.create)
    ..hasRequiredFields = false;

  @$core.Deprecated('Using this can add significant overhead to your binary. '
      'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
      'Will be removed in next major version')
  Message clone() => Message()..mergeFromMessage(this);
  @$core.Deprecated('Using this can add significant overhead to your binary. '
      'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
      'Will be removed in next major version')
  Message copyWith(void Function(Message) updates) =>
      super.copyWith((message) => updates(message as Message)) as Message;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Message create() => Message._();
  Message createEmptyInstance() => create();
  static $pb.PbList<Message> createRepeated() => $pb.PbList<Message>();
  @$core.pragma('dart2js:noInline')
  static Message getDefault() =>
      _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Message>(create);
  static Message? _defaultInstance;

  Message_Message whichMessage() => _Message_MessageByTag[$_whichOneof(0)]!;
  void clearMessage() => clearField($_whichOneof(0));

  @$pb.TagNumber(1)
  Update get update => $_getN(0);
  @$pb.TagNumber(1)
  set update(Update v) {
    setField(1, v);
  }

  @$pb.TagNumber(1)
  $core.bool hasUpdate() => $_has(0);
  @$pb.TagNumber(1)
  void clearUpdate() => clearField(1);
  @$pb.TagNumber(1)
  Update ensureUpdate() => $_ensure(0);

  @$pb.TagNumber(2)
  Initialise get initialise => $_getN(1);
  @$pb.TagNumber(2)
  set initialise(Initialise v) {
    setField(2, v);
  }

  @$pb.TagNumber(2)
  $core.bool hasInitialise() => $_has(1);
  @$pb.TagNumber(2)
  void clearInitialise() => clearField(2);
  @$pb.TagNumber(2)
  Initialise ensureInitialise() => $_ensure(1);
}

const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames =
    $core.bool.fromEnvironment('protobuf.omit_message_names');
