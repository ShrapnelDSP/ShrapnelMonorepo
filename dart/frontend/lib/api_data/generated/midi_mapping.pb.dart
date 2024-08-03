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

import 'midi_mapping.pbenum.dart';

export 'midi_mapping.pbenum.dart';

class Mapping extends $pb.GeneratedMessage {
  factory Mapping({
    $core.int? midiChannel,
    $core.int? ccNumber,
    Mapping_Mode? mode,
    $core.String? parameterName,
    $core.int? presetId,
  }) {
    final $result = create();
    if (midiChannel != null) {
      $result.midiChannel = midiChannel;
    }
    if (ccNumber != null) {
      $result.ccNumber = ccNumber;
    }
    if (mode != null) {
      $result.mode = mode;
    }
    if (parameterName != null) {
      $result.parameterName = parameterName;
    }
    if (presetId != null) {
      $result.presetId = presetId;
    }
    return $result;
  }
  Mapping._() : super();
  factory Mapping.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Mapping.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Mapping', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'midiChannel', $pb.PbFieldType.OU3)
    ..a<$core.int>(2, _omitFieldNames ? '' : 'ccNumber', $pb.PbFieldType.OU3)
    ..e<Mapping_Mode>(3, _omitFieldNames ? '' : 'mode', $pb.PbFieldType.OE, defaultOrMaker: Mapping_Mode.parameter, valueOf: Mapping_Mode.valueOf, enumValues: Mapping_Mode.values)
    ..aOS(4, _omitFieldNames ? '' : 'parameterName')
    ..a<$core.int>(5, _omitFieldNames ? '' : 'presetId', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Mapping clone() => Mapping()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Mapping copyWith(void Function(Mapping) updates) => super.copyWith((message) => updates(message as Mapping)) as Mapping;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Mapping create() => Mapping._();
  Mapping createEmptyInstance() => create();
  static $pb.PbList<Mapping> createRepeated() => $pb.PbList<Mapping>();
  @$core.pragma('dart2js:noInline')
  static Mapping getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Mapping>(create);
  static Mapping? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get midiChannel => $_getIZ(0);
  @$pb.TagNumber(1)
  set midiChannel($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasMidiChannel() => $_has(0);
  @$pb.TagNumber(1)
  void clearMidiChannel() => clearField(1);

  @$pb.TagNumber(2)
  $core.int get ccNumber => $_getIZ(1);
  @$pb.TagNumber(2)
  set ccNumber($core.int v) { $_setUnsignedInt32(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasCcNumber() => $_has(1);
  @$pb.TagNumber(2)
  void clearCcNumber() => clearField(2);

  @$pb.TagNumber(3)
  Mapping_Mode get mode => $_getN(2);
  @$pb.TagNumber(3)
  set mode(Mapping_Mode v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasMode() => $_has(2);
  @$pb.TagNumber(3)
  void clearMode() => clearField(3);

  @$pb.TagNumber(4)
  $core.String get parameterName => $_getSZ(3);
  @$pb.TagNumber(4)
  set parameterName($core.String v) { $_setString(3, v); }
  @$pb.TagNumber(4)
  $core.bool hasParameterName() => $_has(3);
  @$pb.TagNumber(4)
  void clearParameterName() => clearField(4);

  @$pb.TagNumber(5)
  $core.int get presetId => $_getIZ(4);
  @$pb.TagNumber(5)
  set presetId($core.int v) { $_setUnsignedInt32(4, v); }
  @$pb.TagNumber(5)
  $core.bool hasPresetId() => $_has(4);
  @$pb.TagNumber(5)
  void clearPresetId() => clearField(5);
}

class MidiMessage_NoteOn extends $pb.GeneratedMessage {
  factory MidiMessage_NoteOn({
    $core.int? note,
    $core.int? velocity,
  }) {
    final $result = create();
    if (note != null) {
      $result.note = note;
    }
    if (velocity != null) {
      $result.velocity = velocity;
    }
    return $result;
  }
  MidiMessage_NoteOn._() : super();
  factory MidiMessage_NoteOn.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MidiMessage_NoteOn.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MidiMessage.NoteOn', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'note', $pb.PbFieldType.OU3)
    ..a<$core.int>(2, _omitFieldNames ? '' : 'velocity', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MidiMessage_NoteOn clone() => MidiMessage_NoteOn()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MidiMessage_NoteOn copyWith(void Function(MidiMessage_NoteOn) updates) => super.copyWith((message) => updates(message as MidiMessage_NoteOn)) as MidiMessage_NoteOn;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MidiMessage_NoteOn create() => MidiMessage_NoteOn._();
  MidiMessage_NoteOn createEmptyInstance() => create();
  static $pb.PbList<MidiMessage_NoteOn> createRepeated() => $pb.PbList<MidiMessage_NoteOn>();
  @$core.pragma('dart2js:noInline')
  static MidiMessage_NoteOn getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MidiMessage_NoteOn>(create);
  static MidiMessage_NoteOn? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get note => $_getIZ(0);
  @$pb.TagNumber(1)
  set note($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasNote() => $_has(0);
  @$pb.TagNumber(1)
  void clearNote() => clearField(1);

  @$pb.TagNumber(2)
  $core.int get velocity => $_getIZ(1);
  @$pb.TagNumber(2)
  set velocity($core.int v) { $_setUnsignedInt32(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasVelocity() => $_has(1);
  @$pb.TagNumber(2)
  void clearVelocity() => clearField(2);
}

class MidiMessage_NoteOff extends $pb.GeneratedMessage {
  factory MidiMessage_NoteOff({
    $core.int? note,
    $core.int? velocity,
  }) {
    final $result = create();
    if (note != null) {
      $result.note = note;
    }
    if (velocity != null) {
      $result.velocity = velocity;
    }
    return $result;
  }
  MidiMessage_NoteOff._() : super();
  factory MidiMessage_NoteOff.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MidiMessage_NoteOff.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MidiMessage.NoteOff', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'note', $pb.PbFieldType.OU3)
    ..a<$core.int>(2, _omitFieldNames ? '' : 'velocity', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MidiMessage_NoteOff clone() => MidiMessage_NoteOff()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MidiMessage_NoteOff copyWith(void Function(MidiMessage_NoteOff) updates) => super.copyWith((message) => updates(message as MidiMessage_NoteOff)) as MidiMessage_NoteOff;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MidiMessage_NoteOff create() => MidiMessage_NoteOff._();
  MidiMessage_NoteOff createEmptyInstance() => create();
  static $pb.PbList<MidiMessage_NoteOff> createRepeated() => $pb.PbList<MidiMessage_NoteOff>();
  @$core.pragma('dart2js:noInline')
  static MidiMessage_NoteOff getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MidiMessage_NoteOff>(create);
  static MidiMessage_NoteOff? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get note => $_getIZ(0);
  @$pb.TagNumber(1)
  set note($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasNote() => $_has(0);
  @$pb.TagNumber(1)
  void clearNote() => clearField(1);

  @$pb.TagNumber(2)
  $core.int get velocity => $_getIZ(1);
  @$pb.TagNumber(2)
  set velocity($core.int v) { $_setUnsignedInt32(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasVelocity() => $_has(1);
  @$pb.TagNumber(2)
  void clearVelocity() => clearField(2);
}

class MidiMessage_ControlChange extends $pb.GeneratedMessage {
  factory MidiMessage_ControlChange({
    $core.int? control,
    $core.int? value,
  }) {
    final $result = create();
    if (control != null) {
      $result.control = control;
    }
    if (value != null) {
      $result.value = value;
    }
    return $result;
  }
  MidiMessage_ControlChange._() : super();
  factory MidiMessage_ControlChange.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MidiMessage_ControlChange.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MidiMessage.ControlChange', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'control', $pb.PbFieldType.OU3)
    ..a<$core.int>(2, _omitFieldNames ? '' : 'value', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MidiMessage_ControlChange clone() => MidiMessage_ControlChange()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MidiMessage_ControlChange copyWith(void Function(MidiMessage_ControlChange) updates) => super.copyWith((message) => updates(message as MidiMessage_ControlChange)) as MidiMessage_ControlChange;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MidiMessage_ControlChange create() => MidiMessage_ControlChange._();
  MidiMessage_ControlChange createEmptyInstance() => create();
  static $pb.PbList<MidiMessage_ControlChange> createRepeated() => $pb.PbList<MidiMessage_ControlChange>();
  @$core.pragma('dart2js:noInline')
  static MidiMessage_ControlChange getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MidiMessage_ControlChange>(create);
  static MidiMessage_ControlChange? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get control => $_getIZ(0);
  @$pb.TagNumber(1)
  set control($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasControl() => $_has(0);
  @$pb.TagNumber(1)
  void clearControl() => clearField(1);

  @$pb.TagNumber(2)
  $core.int get value => $_getIZ(1);
  @$pb.TagNumber(2)
  set value($core.int v) { $_setUnsignedInt32(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasValue() => $_has(1);
  @$pb.TagNumber(2)
  void clearValue() => clearField(2);
}

class MidiMessage_ProgramChange extends $pb.GeneratedMessage {
  factory MidiMessage_ProgramChange({
    $core.int? number,
  }) {
    final $result = create();
    if (number != null) {
      $result.number = number;
    }
    return $result;
  }
  MidiMessage_ProgramChange._() : super();
  factory MidiMessage_ProgramChange.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MidiMessage_ProgramChange.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MidiMessage.ProgramChange', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'number', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MidiMessage_ProgramChange clone() => MidiMessage_ProgramChange()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MidiMessage_ProgramChange copyWith(void Function(MidiMessage_ProgramChange) updates) => super.copyWith((message) => updates(message as MidiMessage_ProgramChange)) as MidiMessage_ProgramChange;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MidiMessage_ProgramChange create() => MidiMessage_ProgramChange._();
  MidiMessage_ProgramChange createEmptyInstance() => create();
  static $pb.PbList<MidiMessage_ProgramChange> createRepeated() => $pb.PbList<MidiMessage_ProgramChange>();
  @$core.pragma('dart2js:noInline')
  static MidiMessage_ProgramChange getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MidiMessage_ProgramChange>(create);
  static MidiMessage_ProgramChange? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get number => $_getIZ(0);
  @$pb.TagNumber(1)
  set number($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasNumber() => $_has(0);
  @$pb.TagNumber(1)
  void clearNumber() => clearField(1);
}

enum MidiMessage_Parameters {
  noteOn, 
  noteOff, 
  controlChange, 
  programChange, 
  notSet
}

/// A MIDI message that was received by the firmware
class MidiMessage extends $pb.GeneratedMessage {
  factory MidiMessage({
    $core.int? channel,
    MidiMessage_NoteOn? noteOn,
    MidiMessage_NoteOff? noteOff,
    MidiMessage_ControlChange? controlChange,
    MidiMessage_ProgramChange? programChange,
  }) {
    final $result = create();
    if (channel != null) {
      $result.channel = channel;
    }
    if (noteOn != null) {
      $result.noteOn = noteOn;
    }
    if (noteOff != null) {
      $result.noteOff = noteOff;
    }
    if (controlChange != null) {
      $result.controlChange = controlChange;
    }
    if (programChange != null) {
      $result.programChange = programChange;
    }
    return $result;
  }
  MidiMessage._() : super();
  factory MidiMessage.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MidiMessage.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, MidiMessage_Parameters> _MidiMessage_ParametersByTag = {
    2 : MidiMessage_Parameters.noteOn,
    3 : MidiMessage_Parameters.noteOff,
    4 : MidiMessage_Parameters.controlChange,
    5 : MidiMessage_Parameters.programChange,
    0 : MidiMessage_Parameters.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MidiMessage', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..oo(0, [2, 3, 4, 5])
    ..a<$core.int>(1, _omitFieldNames ? '' : 'channel', $pb.PbFieldType.OU3)
    ..aOM<MidiMessage_NoteOn>(2, _omitFieldNames ? '' : 'noteOn', subBuilder: MidiMessage_NoteOn.create)
    ..aOM<MidiMessage_NoteOff>(3, _omitFieldNames ? '' : 'noteOff', subBuilder: MidiMessage_NoteOff.create)
    ..aOM<MidiMessage_ControlChange>(4, _omitFieldNames ? '' : 'controlChange', subBuilder: MidiMessage_ControlChange.create)
    ..aOM<MidiMessage_ProgramChange>(5, _omitFieldNames ? '' : 'programChange', subBuilder: MidiMessage_ProgramChange.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MidiMessage clone() => MidiMessage()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MidiMessage copyWith(void Function(MidiMessage) updates) => super.copyWith((message) => updates(message as MidiMessage)) as MidiMessage;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MidiMessage create() => MidiMessage._();
  MidiMessage createEmptyInstance() => create();
  static $pb.PbList<MidiMessage> createRepeated() => $pb.PbList<MidiMessage>();
  @$core.pragma('dart2js:noInline')
  static MidiMessage getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MidiMessage>(create);
  static MidiMessage? _defaultInstance;

  MidiMessage_Parameters whichParameters() => _MidiMessage_ParametersByTag[$_whichOneof(0)]!;
  void clearParameters() => clearField($_whichOneof(0));

  /// The MIDI channel where the message was received
  @$pb.TagNumber(1)
  $core.int get channel => $_getIZ(0);
  @$pb.TagNumber(1)
  set channel($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasChannel() => $_has(0);
  @$pb.TagNumber(1)
  void clearChannel() => clearField(1);

  @$pb.TagNumber(2)
  MidiMessage_NoteOn get noteOn => $_getN(1);
  @$pb.TagNumber(2)
  set noteOn(MidiMessage_NoteOn v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasNoteOn() => $_has(1);
  @$pb.TagNumber(2)
  void clearNoteOn() => clearField(2);
  @$pb.TagNumber(2)
  MidiMessage_NoteOn ensureNoteOn() => $_ensure(1);

  @$pb.TagNumber(3)
  MidiMessage_NoteOff get noteOff => $_getN(2);
  @$pb.TagNumber(3)
  set noteOff(MidiMessage_NoteOff v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasNoteOff() => $_has(2);
  @$pb.TagNumber(3)
  void clearNoteOff() => clearField(3);
  @$pb.TagNumber(3)
  MidiMessage_NoteOff ensureNoteOff() => $_ensure(2);

  @$pb.TagNumber(4)
  MidiMessage_ControlChange get controlChange => $_getN(3);
  @$pb.TagNumber(4)
  set controlChange(MidiMessage_ControlChange v) { setField(4, v); }
  @$pb.TagNumber(4)
  $core.bool hasControlChange() => $_has(3);
  @$pb.TagNumber(4)
  void clearControlChange() => clearField(4);
  @$pb.TagNumber(4)
  MidiMessage_ControlChange ensureControlChange() => $_ensure(3);

  @$pb.TagNumber(5)
  MidiMessage_ProgramChange get programChange => $_getN(4);
  @$pb.TagNumber(5)
  set programChange(MidiMessage_ProgramChange v) { setField(5, v); }
  @$pb.TagNumber(5)
  $core.bool hasProgramChange() => $_has(4);
  @$pb.TagNumber(5)
  void clearProgramChange() => clearField(5);
  @$pb.TagNumber(5)
  MidiMessage_ProgramChange ensureProgramChange() => $_ensure(4);
}

class MappingRecord extends $pb.GeneratedMessage {
  factory MappingRecord({
    $core.int? id,
    Mapping? mapping,
  }) {
    final $result = create();
    if (id != null) {
      $result.id = id;
    }
    if (mapping != null) {
      $result.mapping = mapping;
    }
    return $result;
  }
  MappingRecord._() : super();
  factory MappingRecord.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MappingRecord.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MappingRecord', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'id', $pb.PbFieldType.OU3)
    ..aOM<Mapping>(2, _omitFieldNames ? '' : 'mapping', subBuilder: Mapping.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MappingRecord clone() => MappingRecord()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MappingRecord copyWith(void Function(MappingRecord) updates) => super.copyWith((message) => updates(message as MappingRecord)) as MappingRecord;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MappingRecord create() => MappingRecord._();
  MappingRecord createEmptyInstance() => create();
  static $pb.PbList<MappingRecord> createRepeated() => $pb.PbList<MappingRecord>();
  @$core.pragma('dart2js:noInline')
  static MappingRecord getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MappingRecord>(create);
  static MappingRecord? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => clearField(1);

  @$pb.TagNumber(2)
  Mapping get mapping => $_getN(1);
  @$pb.TagNumber(2)
  set mapping(Mapping v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasMapping() => $_has(1);
  @$pb.TagNumber(2)
  void clearMapping() => clearField(2);
  @$pb.TagNumber(2)
  Mapping ensureMapping() => $_ensure(1);
}

/// Send from the frontend to the firmware. When received, the firmware will send
/// an update for each midi mapping that currently exists.
class GetRequest extends $pb.GeneratedMessage {
  factory GetRequest() => create();
  GetRequest._() : super();
  factory GetRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory GetRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'GetRequest', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  GetRequest clone() => GetRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  GetRequest copyWith(void Function(GetRequest) updates) => super.copyWith((message) => updates(message as GetRequest)) as GetRequest;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static GetRequest create() => GetRequest._();
  GetRequest createEmptyInstance() => create();
  static $pb.PbList<GetRequest> createRepeated() => $pb.PbList<GetRequest>();
  @$core.pragma('dart2js:noInline')
  static GetRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<GetRequest>(create);
  static GetRequest? _defaultInstance;
}

/// Firmware creates a new MIDI mapping according to the parameters.
class CreateRequest extends $pb.GeneratedMessage {
  factory CreateRequest({
    Mapping? mapping,
  }) {
    final $result = create();
    if (mapping != null) {
      $result.mapping = mapping;
    }
    return $result;
  }
  CreateRequest._() : super();
  factory CreateRequest.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory CreateRequest.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'CreateRequest', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..aOM<Mapping>(1, _omitFieldNames ? '' : 'mapping', subBuilder: Mapping.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  CreateRequest clone() => CreateRequest()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  CreateRequest copyWith(void Function(CreateRequest) updates) => super.copyWith((message) => updates(message as CreateRequest)) as CreateRequest;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static CreateRequest create() => CreateRequest._();
  CreateRequest createEmptyInstance() => create();
  static $pb.PbList<CreateRequest> createRepeated() => $pb.PbList<CreateRequest>();
  @$core.pragma('dart2js:noInline')
  static CreateRequest getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<CreateRequest>(create);
  static CreateRequest? _defaultInstance;

  @$pb.TagNumber(1)
  Mapping get mapping => $_getN(0);
  @$pb.TagNumber(1)
  set mapping(Mapping v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasMapping() => $_has(0);
  @$pb.TagNumber(1)
  void clearMapping() => clearField(1);
  @$pb.TagNumber(1)
  Mapping ensureMapping() => $_ensure(0);
}

/// Indicates that a new MIDI mapping was created successfully.
class CreateResponse extends $pb.GeneratedMessage {
  factory CreateResponse({
    MappingRecord? mapping,
  }) {
    final $result = create();
    if (mapping != null) {
      $result.mapping = mapping;
    }
    return $result;
  }
  CreateResponse._() : super();
  factory CreateResponse.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory CreateResponse.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'CreateResponse', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..aOM<MappingRecord>(1, _omitFieldNames ? '' : 'mapping', subBuilder: MappingRecord.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  CreateResponse clone() => CreateResponse()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  CreateResponse copyWith(void Function(CreateResponse) updates) => super.copyWith((message) => updates(message as CreateResponse)) as CreateResponse;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static CreateResponse create() => CreateResponse._();
  CreateResponse createEmptyInstance() => create();
  static $pb.PbList<CreateResponse> createRepeated() => $pb.PbList<CreateResponse>();
  @$core.pragma('dart2js:noInline')
  static CreateResponse getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<CreateResponse>(create);
  static CreateResponse? _defaultInstance;

  @$pb.TagNumber(1)
  MappingRecord get mapping => $_getN(0);
  @$pb.TagNumber(1)
  set mapping(MappingRecord v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasMapping() => $_has(0);
  @$pb.TagNumber(1)
  void clearMapping() => clearField(1);
  @$pb.TagNumber(1)
  MappingRecord ensureMapping() => $_ensure(0);
}

/// Firmware updates an existing MIDI mapping according to the parameters.
class Update extends $pb.GeneratedMessage {
  factory Update({
    MappingRecord? mapping,
  }) {
    final $result = create();
    if (mapping != null) {
      $result.mapping = mapping;
    }
    return $result;
  }
  Update._() : super();
  factory Update.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Update.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Update', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..aOM<MappingRecord>(1, _omitFieldNames ? '' : 'mapping', subBuilder: MappingRecord.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Update clone() => Update()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Update copyWith(void Function(Update) updates) => super.copyWith((message) => updates(message as Update)) as Update;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Update create() => Update._();
  Update createEmptyInstance() => create();
  static $pb.PbList<Update> createRepeated() => $pb.PbList<Update>();
  @$core.pragma('dart2js:noInline')
  static Update getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Update>(create);
  static Update? _defaultInstance;

  @$pb.TagNumber(1)
  MappingRecord get mapping => $_getN(0);
  @$pb.TagNumber(1)
  set mapping(MappingRecord v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasMapping() => $_has(0);
  @$pb.TagNumber(1)
  void clearMapping() => clearField(1);
  @$pb.TagNumber(1)
  MappingRecord ensureMapping() => $_ensure(0);
}

/// Firmware removes the specified MIDI mapping .
class Remove extends $pb.GeneratedMessage {
  factory Remove({
    $core.int? id,
  }) {
    final $result = create();
    if (id != null) {
      $result.id = id;
    }
    return $result;
  }
  Remove._() : super();
  factory Remove.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Remove.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Remove', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'id', $pb.PbFieldType.OU3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Remove clone() => Remove()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Remove copyWith(void Function(Remove) updates) => super.copyWith((message) => updates(message as Remove)) as Remove;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Remove create() => Remove._();
  Remove createEmptyInstance() => create();
  static $pb.PbList<Remove> createRepeated() => $pb.PbList<Remove>();
  @$core.pragma('dart2js:noInline')
  static Remove getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Remove>(create);
  static Remove? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => clearField(1);
}

/// Firmware notifies this periodically, so that the frontend can react
/// to MIDI messages, for example to connect a parameter to the MIDI
/// message.
class MessageReceived extends $pb.GeneratedMessage {
  factory MessageReceived({
    MidiMessage? receivedMessage,
  }) {
    final $result = create();
    if (receivedMessage != null) {
      $result.receivedMessage = receivedMessage;
    }
    return $result;
  }
  MessageReceived._() : super();
  factory MessageReceived.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory MessageReceived.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'MessageReceived', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..aOM<MidiMessage>(1, _omitFieldNames ? '' : 'receivedMessage', subBuilder: MidiMessage.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  MessageReceived clone() => MessageReceived()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  MessageReceived copyWith(void Function(MessageReceived) updates) => super.copyWith((message) => updates(message as MessageReceived)) as MessageReceived;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static MessageReceived create() => MessageReceived._();
  MessageReceived createEmptyInstance() => create();
  static $pb.PbList<MessageReceived> createRepeated() => $pb.PbList<MessageReceived>();
  @$core.pragma('dart2js:noInline')
  static MessageReceived getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<MessageReceived>(create);
  static MessageReceived? _defaultInstance;

  @$pb.TagNumber(1)
  MidiMessage get receivedMessage => $_getN(0);
  @$pb.TagNumber(1)
  set receivedMessage(MidiMessage v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasReceivedMessage() => $_has(0);
  @$pb.TagNumber(1)
  void clearReceivedMessage() => clearField(1);
  @$pb.TagNumber(1)
  MidiMessage ensureReceivedMessage() => $_ensure(0);
}

enum Message_Message {
  getRequest, 
  createRequest, 
  createResponse, 
  update, 
  remove, 
  messageReceived, 
  notSet
}

class Message extends $pb.GeneratedMessage {
  factory Message({
    GetRequest? getRequest,
    CreateRequest? createRequest,
    CreateResponse? createResponse,
    Update? update,
    Remove? remove,
    MessageReceived? messageReceived,
  }) {
    final $result = create();
    if (getRequest != null) {
      $result.getRequest = getRequest;
    }
    if (createRequest != null) {
      $result.createRequest = createRequest;
    }
    if (createResponse != null) {
      $result.createResponse = createResponse;
    }
    if (update != null) {
      $result.update = update;
    }
    if (remove != null) {
      $result.remove = remove;
    }
    if (messageReceived != null) {
      $result.messageReceived = messageReceived;
    }
    return $result;
  }
  Message._() : super();
  factory Message.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Message.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, Message_Message> _Message_MessageByTag = {
    1 : Message_Message.getRequest,
    2 : Message_Message.createRequest,
    3 : Message_Message.createResponse,
    4 : Message_Message.update,
    5 : Message_Message.remove,
    6 : Message_Message.messageReceived,
    0 : Message_Message.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Message', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.midi_mapping'), createEmptyInstance: create)
    ..oo(0, [1, 2, 3, 4, 5, 6])
    ..aOM<GetRequest>(1, _omitFieldNames ? '' : 'getRequest', subBuilder: GetRequest.create)
    ..aOM<CreateRequest>(2, _omitFieldNames ? '' : 'createRequest', subBuilder: CreateRequest.create)
    ..aOM<CreateResponse>(3, _omitFieldNames ? '' : 'createResponse', subBuilder: CreateResponse.create)
    ..aOM<Update>(4, _omitFieldNames ? '' : 'update', subBuilder: Update.create)
    ..aOM<Remove>(5, _omitFieldNames ? '' : 'remove', subBuilder: Remove.create)
    ..aOM<MessageReceived>(6, _omitFieldNames ? '' : 'messageReceived', subBuilder: MessageReceived.create)
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
  GetRequest get getRequest => $_getN(0);
  @$pb.TagNumber(1)
  set getRequest(GetRequest v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasGetRequest() => $_has(0);
  @$pb.TagNumber(1)
  void clearGetRequest() => clearField(1);
  @$pb.TagNumber(1)
  GetRequest ensureGetRequest() => $_ensure(0);

  @$pb.TagNumber(2)
  CreateRequest get createRequest => $_getN(1);
  @$pb.TagNumber(2)
  set createRequest(CreateRequest v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasCreateRequest() => $_has(1);
  @$pb.TagNumber(2)
  void clearCreateRequest() => clearField(2);
  @$pb.TagNumber(2)
  CreateRequest ensureCreateRequest() => $_ensure(1);

  @$pb.TagNumber(3)
  CreateResponse get createResponse => $_getN(2);
  @$pb.TagNumber(3)
  set createResponse(CreateResponse v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasCreateResponse() => $_has(2);
  @$pb.TagNumber(3)
  void clearCreateResponse() => clearField(3);
  @$pb.TagNumber(3)
  CreateResponse ensureCreateResponse() => $_ensure(2);

  @$pb.TagNumber(4)
  Update get update => $_getN(3);
  @$pb.TagNumber(4)
  set update(Update v) { setField(4, v); }
  @$pb.TagNumber(4)
  $core.bool hasUpdate() => $_has(3);
  @$pb.TagNumber(4)
  void clearUpdate() => clearField(4);
  @$pb.TagNumber(4)
  Update ensureUpdate() => $_ensure(3);

  @$pb.TagNumber(5)
  Remove get remove => $_getN(4);
  @$pb.TagNumber(5)
  set remove(Remove v) { setField(5, v); }
  @$pb.TagNumber(5)
  $core.bool hasRemove() => $_has(4);
  @$pb.TagNumber(5)
  void clearRemove() => clearField(5);
  @$pb.TagNumber(5)
  Remove ensureRemove() => $_ensure(4);

  @$pb.TagNumber(6)
  MessageReceived get messageReceived => $_getN(5);
  @$pb.TagNumber(6)
  set messageReceived(MessageReceived v) { setField(6, v); }
  @$pb.TagNumber(6)
  $core.bool hasMessageReceived() => $_has(5);
  @$pb.TagNumber(6)
  void clearMessageReceived() => clearField(6);
  @$pb.TagNumber(6)
  MessageReceived ensureMessageReceived() => $_ensure(5);
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
