//
//  Generated code. Do not modify.
//  source: shrapnel.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

import 'audio_events.pb.dart' as $1;
import 'cmd_handling.pb.dart' as $2;
import 'midi_mapping.pb.dart' as $3;
import 'presets.pb.dart' as $4;
import 'selected_preset.pb.dart' as $5;

enum Message_Message {
  audioEvent, 
  command, 
  midiMapping, 
  preset, 
  selectedPreset, 
  notSet
}

class Message extends $pb.GeneratedMessage {
  factory Message({
    $1.Message? audioEvent,
    $2.Message? command,
    $3.Message? midiMapping,
    $4.Message? preset,
    $5.Message? selectedPreset,
  }) {
    final $result = create();
    if (audioEvent != null) {
      $result.audioEvent = audioEvent;
    }
    if (command != null) {
      $result.command = command;
    }
    if (midiMapping != null) {
      $result.midiMapping = midiMapping;
    }
    if (preset != null) {
      $result.preset = preset;
    }
    if (selectedPreset != null) {
      $result.selectedPreset = selectedPreset;
    }
    return $result;
  }
  Message._() : super();
  factory Message.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Message.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, Message_Message> _Message_MessageByTag = {
    1 : Message_Message.audioEvent,
    2 : Message_Message.command,
    3 : Message_Message.midiMapping,
    4 : Message_Message.preset,
    5 : Message_Message.selectedPreset,
    0 : Message_Message.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Message', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.messages'), createEmptyInstance: create)
    ..oo(0, [1, 2, 3, 4, 5])
    ..aOM<$1.Message>(1, _omitFieldNames ? '' : 'audioEvent', subBuilder: $1.Message.create)
    ..aOM<$2.Message>(2, _omitFieldNames ? '' : 'command', subBuilder: $2.Message.create)
    ..aOM<$3.Message>(3, _omitFieldNames ? '' : 'midiMapping', subBuilder: $3.Message.create)
    ..aOM<$4.Message>(4, _omitFieldNames ? '' : 'preset', subBuilder: $4.Message.create)
    ..aOM<$5.Message>(5, _omitFieldNames ? '' : 'selectedPreset', subBuilder: $5.Message.create)
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
  $1.Message get audioEvent => $_getN(0);
  @$pb.TagNumber(1)
  set audioEvent($1.Message v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasAudioEvent() => $_has(0);
  @$pb.TagNumber(1)
  void clearAudioEvent() => clearField(1);
  @$pb.TagNumber(1)
  $1.Message ensureAudioEvent() => $_ensure(0);

  @$pb.TagNumber(2)
  $2.Message get command => $_getN(1);
  @$pb.TagNumber(2)
  set command($2.Message v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasCommand() => $_has(1);
  @$pb.TagNumber(2)
  void clearCommand() => clearField(2);
  @$pb.TagNumber(2)
  $2.Message ensureCommand() => $_ensure(1);

  @$pb.TagNumber(3)
  $3.Message get midiMapping => $_getN(2);
  @$pb.TagNumber(3)
  set midiMapping($3.Message v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasMidiMapping() => $_has(2);
  @$pb.TagNumber(3)
  void clearMidiMapping() => clearField(3);
  @$pb.TagNumber(3)
  $3.Message ensureMidiMapping() => $_ensure(2);

  @$pb.TagNumber(4)
  $4.Message get preset => $_getN(3);
  @$pb.TagNumber(4)
  set preset($4.Message v) { setField(4, v); }
  @$pb.TagNumber(4)
  $core.bool hasPreset() => $_has(3);
  @$pb.TagNumber(4)
  void clearPreset() => clearField(4);
  @$pb.TagNumber(4)
  $4.Message ensurePreset() => $_ensure(3);

  @$pb.TagNumber(5)
  $5.Message get selectedPreset => $_getN(4);
  @$pb.TagNumber(5)
  set selectedPreset($5.Message v) { setField(5, v); }
  @$pb.TagNumber(5)
  $core.bool hasSelectedPreset() => $_has(4);
  @$pb.TagNumber(5)
  void clearSelectedPreset() => clearField(5);
  @$pb.TagNumber(5)
  $5.Message ensureSelectedPreset() => $_ensure(4);
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
