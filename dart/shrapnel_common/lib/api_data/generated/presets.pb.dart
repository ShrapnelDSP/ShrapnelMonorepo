//
//  Generated code. Do not modify.
//  source: presets.proto
//
// @dart = 2.12

// ignore_for_file: annotate_overrides, camel_case_types, comment_references
// ignore_for_file: constant_identifier_names, library_prefixes
// ignore_for_file: non_constant_identifier_names, prefer_final_fields
// ignore_for_file: unnecessary_import, unnecessary_this, unused_import

import 'dart:core' as $core;

import 'package:protobuf/protobuf.dart' as $pb;

class PresetParameters extends $pb.GeneratedMessage {
  factory PresetParameters({
    $core.int? ampGain,
    $core.int? ampChannel,
    $core.int? bass,
    $core.int? middle,
    $core.int? treble,
    $core.int? contour,
    $core.int? volume,
    $core.int? noiseGateThreshold,
    $core.int? noiseGateHysteresis,
    $core.int? noiseGateAttack,
    $core.int? noiseGateHold,
    $core.int? noiseGateRelease,
    $core.int? noiseGateBypass,
    $core.int? chorusRate,
    $core.int? chorusDepth,
    $core.int? chorusMix,
    $core.int? chorusBypass,
    $core.int? wahPosition,
    $core.int? wahVocal,
    $core.int? wahBypass,
  }) {
    final $result = create();
    if (ampGain != null) {
      $result.ampGain = ampGain;
    }
    if (ampChannel != null) {
      $result.ampChannel = ampChannel;
    }
    if (bass != null) {
      $result.bass = bass;
    }
    if (middle != null) {
      $result.middle = middle;
    }
    if (treble != null) {
      $result.treble = treble;
    }
    if (contour != null) {
      $result.contour = contour;
    }
    if (volume != null) {
      $result.volume = volume;
    }
    if (noiseGateThreshold != null) {
      $result.noiseGateThreshold = noiseGateThreshold;
    }
    if (noiseGateHysteresis != null) {
      $result.noiseGateHysteresis = noiseGateHysteresis;
    }
    if (noiseGateAttack != null) {
      $result.noiseGateAttack = noiseGateAttack;
    }
    if (noiseGateHold != null) {
      $result.noiseGateHold = noiseGateHold;
    }
    if (noiseGateRelease != null) {
      $result.noiseGateRelease = noiseGateRelease;
    }
    if (noiseGateBypass != null) {
      $result.noiseGateBypass = noiseGateBypass;
    }
    if (chorusRate != null) {
      $result.chorusRate = chorusRate;
    }
    if (chorusDepth != null) {
      $result.chorusDepth = chorusDepth;
    }
    if (chorusMix != null) {
      $result.chorusMix = chorusMix;
    }
    if (chorusBypass != null) {
      $result.chorusBypass = chorusBypass;
    }
    if (wahPosition != null) {
      $result.wahPosition = wahPosition;
    }
    if (wahVocal != null) {
      $result.wahVocal = wahVocal;
    }
    if (wahBypass != null) {
      $result.wahBypass = wahBypass;
    }
    return $result;
  }
  PresetParameters._() : super();
  factory PresetParameters.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory PresetParameters.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'PresetParameters', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'ampGain', $pb.PbFieldType.O3)
    ..a<$core.int>(2, _omitFieldNames ? '' : 'ampChannel', $pb.PbFieldType.O3)
    ..a<$core.int>(3, _omitFieldNames ? '' : 'bass', $pb.PbFieldType.O3)
    ..a<$core.int>(4, _omitFieldNames ? '' : 'middle', $pb.PbFieldType.O3)
    ..a<$core.int>(5, _omitFieldNames ? '' : 'treble', $pb.PbFieldType.O3)
    ..a<$core.int>(6, _omitFieldNames ? '' : 'contour', $pb.PbFieldType.O3)
    ..a<$core.int>(7, _omitFieldNames ? '' : 'volume', $pb.PbFieldType.O3)
    ..a<$core.int>(8, _omitFieldNames ? '' : 'noiseGateThreshold', $pb.PbFieldType.O3)
    ..a<$core.int>(9, _omitFieldNames ? '' : 'noiseGateHysteresis', $pb.PbFieldType.O3)
    ..a<$core.int>(10, _omitFieldNames ? '' : 'noiseGateAttack', $pb.PbFieldType.O3)
    ..a<$core.int>(11, _omitFieldNames ? '' : 'noiseGateHold', $pb.PbFieldType.O3)
    ..a<$core.int>(12, _omitFieldNames ? '' : 'noiseGateRelease', $pb.PbFieldType.O3)
    ..a<$core.int>(13, _omitFieldNames ? '' : 'noiseGateBypass', $pb.PbFieldType.O3)
    ..a<$core.int>(14, _omitFieldNames ? '' : 'chorusRate', $pb.PbFieldType.O3)
    ..a<$core.int>(15, _omitFieldNames ? '' : 'chorusDepth', $pb.PbFieldType.O3)
    ..a<$core.int>(16, _omitFieldNames ? '' : 'chorusMix', $pb.PbFieldType.O3)
    ..a<$core.int>(17, _omitFieldNames ? '' : 'chorusBypass', $pb.PbFieldType.O3)
    ..a<$core.int>(18, _omitFieldNames ? '' : 'wahPosition', $pb.PbFieldType.O3)
    ..a<$core.int>(19, _omitFieldNames ? '' : 'wahVocal', $pb.PbFieldType.O3)
    ..a<$core.int>(20, _omitFieldNames ? '' : 'wahBypass', $pb.PbFieldType.O3)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  PresetParameters clone() => PresetParameters()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  PresetParameters copyWith(void Function(PresetParameters) updates) => super.copyWith((message) => updates(message as PresetParameters)) as PresetParameters;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static PresetParameters create() => PresetParameters._();
  PresetParameters createEmptyInstance() => create();
  static $pb.PbList<PresetParameters> createRepeated() => $pb.PbList<PresetParameters>();
  @$core.pragma('dart2js:noInline')
  static PresetParameters getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<PresetParameters>(create);
  static PresetParameters? _defaultInstance;

  @$pb.TagNumber(1)
  $core.int get ampGain => $_getIZ(0);
  @$pb.TagNumber(1)
  set ampGain($core.int v) { $_setSignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasAmpGain() => $_has(0);
  @$pb.TagNumber(1)
  void clearAmpGain() => clearField(1);

  @$pb.TagNumber(2)
  $core.int get ampChannel => $_getIZ(1);
  @$pb.TagNumber(2)
  set ampChannel($core.int v) { $_setSignedInt32(1, v); }
  @$pb.TagNumber(2)
  $core.bool hasAmpChannel() => $_has(1);
  @$pb.TagNumber(2)
  void clearAmpChannel() => clearField(2);

  @$pb.TagNumber(3)
  $core.int get bass => $_getIZ(2);
  @$pb.TagNumber(3)
  set bass($core.int v) { $_setSignedInt32(2, v); }
  @$pb.TagNumber(3)
  $core.bool hasBass() => $_has(2);
  @$pb.TagNumber(3)
  void clearBass() => clearField(3);

  @$pb.TagNumber(4)
  $core.int get middle => $_getIZ(3);
  @$pb.TagNumber(4)
  set middle($core.int v) { $_setSignedInt32(3, v); }
  @$pb.TagNumber(4)
  $core.bool hasMiddle() => $_has(3);
  @$pb.TagNumber(4)
  void clearMiddle() => clearField(4);

  @$pb.TagNumber(5)
  $core.int get treble => $_getIZ(4);
  @$pb.TagNumber(5)
  set treble($core.int v) { $_setSignedInt32(4, v); }
  @$pb.TagNumber(5)
  $core.bool hasTreble() => $_has(4);
  @$pb.TagNumber(5)
  void clearTreble() => clearField(5);

  @$pb.TagNumber(6)
  $core.int get contour => $_getIZ(5);
  @$pb.TagNumber(6)
  set contour($core.int v) { $_setSignedInt32(5, v); }
  @$pb.TagNumber(6)
  $core.bool hasContour() => $_has(5);
  @$pb.TagNumber(6)
  void clearContour() => clearField(6);

  @$pb.TagNumber(7)
  $core.int get volume => $_getIZ(6);
  @$pb.TagNumber(7)
  set volume($core.int v) { $_setSignedInt32(6, v); }
  @$pb.TagNumber(7)
  $core.bool hasVolume() => $_has(6);
  @$pb.TagNumber(7)
  void clearVolume() => clearField(7);

  @$pb.TagNumber(8)
  $core.int get noiseGateThreshold => $_getIZ(7);
  @$pb.TagNumber(8)
  set noiseGateThreshold($core.int v) { $_setSignedInt32(7, v); }
  @$pb.TagNumber(8)
  $core.bool hasNoiseGateThreshold() => $_has(7);
  @$pb.TagNumber(8)
  void clearNoiseGateThreshold() => clearField(8);

  @$pb.TagNumber(9)
  $core.int get noiseGateHysteresis => $_getIZ(8);
  @$pb.TagNumber(9)
  set noiseGateHysteresis($core.int v) { $_setSignedInt32(8, v); }
  @$pb.TagNumber(9)
  $core.bool hasNoiseGateHysteresis() => $_has(8);
  @$pb.TagNumber(9)
  void clearNoiseGateHysteresis() => clearField(9);

  @$pb.TagNumber(10)
  $core.int get noiseGateAttack => $_getIZ(9);
  @$pb.TagNumber(10)
  set noiseGateAttack($core.int v) { $_setSignedInt32(9, v); }
  @$pb.TagNumber(10)
  $core.bool hasNoiseGateAttack() => $_has(9);
  @$pb.TagNumber(10)
  void clearNoiseGateAttack() => clearField(10);

  @$pb.TagNumber(11)
  $core.int get noiseGateHold => $_getIZ(10);
  @$pb.TagNumber(11)
  set noiseGateHold($core.int v) { $_setSignedInt32(10, v); }
  @$pb.TagNumber(11)
  $core.bool hasNoiseGateHold() => $_has(10);
  @$pb.TagNumber(11)
  void clearNoiseGateHold() => clearField(11);

  @$pb.TagNumber(12)
  $core.int get noiseGateRelease => $_getIZ(11);
  @$pb.TagNumber(12)
  set noiseGateRelease($core.int v) { $_setSignedInt32(11, v); }
  @$pb.TagNumber(12)
  $core.bool hasNoiseGateRelease() => $_has(11);
  @$pb.TagNumber(12)
  void clearNoiseGateRelease() => clearField(12);

  @$pb.TagNumber(13)
  $core.int get noiseGateBypass => $_getIZ(12);
  @$pb.TagNumber(13)
  set noiseGateBypass($core.int v) { $_setSignedInt32(12, v); }
  @$pb.TagNumber(13)
  $core.bool hasNoiseGateBypass() => $_has(12);
  @$pb.TagNumber(13)
  void clearNoiseGateBypass() => clearField(13);

  @$pb.TagNumber(14)
  $core.int get chorusRate => $_getIZ(13);
  @$pb.TagNumber(14)
  set chorusRate($core.int v) { $_setSignedInt32(13, v); }
  @$pb.TagNumber(14)
  $core.bool hasChorusRate() => $_has(13);
  @$pb.TagNumber(14)
  void clearChorusRate() => clearField(14);

  @$pb.TagNumber(15)
  $core.int get chorusDepth => $_getIZ(14);
  @$pb.TagNumber(15)
  set chorusDepth($core.int v) { $_setSignedInt32(14, v); }
  @$pb.TagNumber(15)
  $core.bool hasChorusDepth() => $_has(14);
  @$pb.TagNumber(15)
  void clearChorusDepth() => clearField(15);

  @$pb.TagNumber(16)
  $core.int get chorusMix => $_getIZ(15);
  @$pb.TagNumber(16)
  set chorusMix($core.int v) { $_setSignedInt32(15, v); }
  @$pb.TagNumber(16)
  $core.bool hasChorusMix() => $_has(15);
  @$pb.TagNumber(16)
  void clearChorusMix() => clearField(16);

  @$pb.TagNumber(17)
  $core.int get chorusBypass => $_getIZ(16);
  @$pb.TagNumber(17)
  set chorusBypass($core.int v) { $_setSignedInt32(16, v); }
  @$pb.TagNumber(17)
  $core.bool hasChorusBypass() => $_has(16);
  @$pb.TagNumber(17)
  void clearChorusBypass() => clearField(17);

  @$pb.TagNumber(18)
  $core.int get wahPosition => $_getIZ(17);
  @$pb.TagNumber(18)
  set wahPosition($core.int v) { $_setSignedInt32(17, v); }
  @$pb.TagNumber(18)
  $core.bool hasWahPosition() => $_has(17);
  @$pb.TagNumber(18)
  void clearWahPosition() => clearField(18);

  @$pb.TagNumber(19)
  $core.int get wahVocal => $_getIZ(18);
  @$pb.TagNumber(19)
  set wahVocal($core.int v) { $_setSignedInt32(18, v); }
  @$pb.TagNumber(19)
  $core.bool hasWahVocal() => $_has(18);
  @$pb.TagNumber(19)
  void clearWahVocal() => clearField(19);

  @$pb.TagNumber(20)
  $core.int get wahBypass => $_getIZ(19);
  @$pb.TagNumber(20)
  set wahBypass($core.int v) { $_setSignedInt32(19, v); }
  @$pb.TagNumber(20)
  $core.bool hasWahBypass() => $_has(19);
  @$pb.TagNumber(20)
  void clearWahBypass() => clearField(20);
}

class Preset extends $pb.GeneratedMessage {
  factory Preset({
    $core.String? name,
    PresetParameters? parameters,
  }) {
    final $result = create();
    if (name != null) {
      $result.name = name;
    }
    if (parameters != null) {
      $result.parameters = parameters;
    }
    return $result;
  }
  Preset._() : super();
  factory Preset.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Preset.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Preset', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..aOS(1, _omitFieldNames ? '' : 'name')
    ..aOM<PresetParameters>(2, _omitFieldNames ? '' : 'parameters', subBuilder: PresetParameters.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Preset clone() => Preset()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Preset copyWith(void Function(Preset) updates) => super.copyWith((message) => updates(message as Preset)) as Preset;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Preset create() => Preset._();
  Preset createEmptyInstance() => create();
  static $pb.PbList<Preset> createRepeated() => $pb.PbList<Preset>();
  @$core.pragma('dart2js:noInline')
  static Preset getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Preset>(create);
  static Preset? _defaultInstance;

  /// The name given by the user to the preset
  @$pb.TagNumber(1)
  $core.String get name => $_getSZ(0);
  @$pb.TagNumber(1)
  set name($core.String v) { $_setString(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasName() => $_has(0);
  @$pb.TagNumber(1)
  void clearName() => clearField(1);

  /// The parameter values for this preset
  @$pb.TagNumber(2)
  PresetParameters get parameters => $_getN(1);
  @$pb.TagNumber(2)
  set parameters(PresetParameters v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasParameters() => $_has(1);
  @$pb.TagNumber(2)
  void clearParameters() => clearField(2);
  @$pb.TagNumber(2)
  PresetParameters ensureParameters() => $_ensure(1);
}

class PresetRecord extends $pb.GeneratedMessage {
  factory PresetRecord({
    $core.int? id,
    Preset? preset,
  }) {
    final $result = create();
    if (id != null) {
      $result.id = id;
    }
    if (preset != null) {
      $result.preset = preset;
    }
    return $result;
  }
  PresetRecord._() : super();
  factory PresetRecord.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory PresetRecord.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'PresetRecord', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..a<$core.int>(1, _omitFieldNames ? '' : 'id', $pb.PbFieldType.OU3)
    ..aOM<Preset>(2, _omitFieldNames ? '' : 'preset', subBuilder: Preset.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  PresetRecord clone() => PresetRecord()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  PresetRecord copyWith(void Function(PresetRecord) updates) => super.copyWith((message) => updates(message as PresetRecord)) as PresetRecord;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static PresetRecord create() => PresetRecord._();
  PresetRecord createEmptyInstance() => create();
  static $pb.PbList<PresetRecord> createRepeated() => $pb.PbList<PresetRecord>();
  @$core.pragma('dart2js:noInline')
  static PresetRecord getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<PresetRecord>(create);
  static PresetRecord? _defaultInstance;

  /// A number used to identify the preset
  @$pb.TagNumber(1)
  $core.int get id => $_getIZ(0);
  @$pb.TagNumber(1)
  set id($core.int v) { $_setUnsignedInt32(0, v); }
  @$pb.TagNumber(1)
  $core.bool hasId() => $_has(0);
  @$pb.TagNumber(1)
  void clearId() => clearField(1);

  @$pb.TagNumber(2)
  Preset get preset => $_getN(1);
  @$pb.TagNumber(2)
  set preset(Preset v) { setField(2, v); }
  @$pb.TagNumber(2)
  $core.bool hasPreset() => $_has(1);
  @$pb.TagNumber(2)
  void clearPreset() => clearField(2);
  @$pb.TagNumber(2)
  Preset ensurePreset() => $_ensure(1);
}

class Initialise extends $pb.GeneratedMessage {
  factory Initialise() => create();
  Initialise._() : super();
  factory Initialise.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Initialise.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Initialise', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Initialise clone() => Initialise()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Initialise copyWith(void Function(Initialise) updates) => super.copyWith((message) => updates(message as Initialise)) as Initialise;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Initialise create() => Initialise._();
  Initialise createEmptyInstance() => create();
  static $pb.PbList<Initialise> createRepeated() => $pb.PbList<Initialise>();
  @$core.pragma('dart2js:noInline')
  static Initialise getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Initialise>(create);
  static Initialise? _defaultInstance;
}

class Notify extends $pb.GeneratedMessage {
  factory Notify({
    PresetRecord? preset,
  }) {
    final $result = create();
    if (preset != null) {
      $result.preset = preset;
    }
    return $result;
  }
  Notify._() : super();
  factory Notify.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Notify.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Notify', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..aOM<PresetRecord>(1, _omitFieldNames ? '' : 'preset', subBuilder: PresetRecord.create)
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
  PresetRecord get preset => $_getN(0);
  @$pb.TagNumber(1)
  set preset(PresetRecord v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasPreset() => $_has(0);
  @$pb.TagNumber(1)
  void clearPreset() => clearField(1);
  @$pb.TagNumber(1)
  PresetRecord ensurePreset() => $_ensure(0);
}

class Create extends $pb.GeneratedMessage {
  factory Create({
    Preset? preset,
  }) {
    final $result = create();
    if (preset != null) {
      $result.preset = preset;
    }
    return $result;
  }
  Create._() : super();
  factory Create.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Create.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Create', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..aOM<Preset>(1, _omitFieldNames ? '' : 'preset', subBuilder: Preset.create)
    ..hasRequiredFields = false
  ;

  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.deepCopy] instead. '
  'Will be removed in next major version')
  Create clone() => Create()..mergeFromMessage(this);
  @$core.Deprecated(
  'Using this can add significant overhead to your binary. '
  'Use [GeneratedMessageGenericExtensions.rebuild] instead. '
  'Will be removed in next major version')
  Create copyWith(void Function(Create) updates) => super.copyWith((message) => updates(message as Create)) as Create;

  $pb.BuilderInfo get info_ => _i;

  @$core.pragma('dart2js:noInline')
  static Create create() => Create._();
  Create createEmptyInstance() => create();
  static $pb.PbList<Create> createRepeated() => $pb.PbList<Create>();
  @$core.pragma('dart2js:noInline')
  static Create getDefault() => _defaultInstance ??= $pb.GeneratedMessage.$_defaultFor<Create>(create);
  static Create? _defaultInstance;

  @$pb.TagNumber(1)
  Preset get preset => $_getN(0);
  @$pb.TagNumber(1)
  set preset(Preset v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasPreset() => $_has(0);
  @$pb.TagNumber(1)
  void clearPreset() => clearField(1);
  @$pb.TagNumber(1)
  Preset ensurePreset() => $_ensure(0);
}

class Update extends $pb.GeneratedMessage {
  factory Update({
    PresetRecord? preset,
  }) {
    final $result = create();
    if (preset != null) {
      $result.preset = preset;
    }
    return $result;
  }
  Update._() : super();
  factory Update.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Update.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Update', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..aOM<PresetRecord>(1, _omitFieldNames ? '' : 'preset', subBuilder: PresetRecord.create)
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
  PresetRecord get preset => $_getN(0);
  @$pb.TagNumber(1)
  set preset(PresetRecord v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasPreset() => $_has(0);
  @$pb.TagNumber(1)
  void clearPreset() => clearField(1);
  @$pb.TagNumber(1)
  PresetRecord ensurePreset() => $_ensure(0);
}

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

  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Remove', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
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

enum Message_Message {
  initialise, 
  notify, 
  create_3, 
  update, 
  remove, 
  notSet
}

class Message extends $pb.GeneratedMessage {
  factory Message({
    Initialise? initialise,
    Notify? notify,
    Create? create_3,
    Update? update,
    Remove? remove,
  }) {
    final $result = create();
    if (initialise != null) {
      $result.initialise = initialise;
    }
    if (notify != null) {
      $result.notify = notify;
    }
    if (create_3 != null) {
      $result.create_3 = create_3;
    }
    if (update != null) {
      $result.update = update;
    }
    if (remove != null) {
      $result.remove = remove;
    }
    return $result;
  }
  Message._() : super();
  factory Message.fromBuffer($core.List<$core.int> i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromBuffer(i, r);
  factory Message.fromJson($core.String i, [$pb.ExtensionRegistry r = $pb.ExtensionRegistry.EMPTY]) => create()..mergeFromJson(i, r);

  static const $core.Map<$core.int, Message_Message> _Message_MessageByTag = {
    1 : Message_Message.initialise,
    2 : Message_Message.notify,
    3 : Message_Message.create_3,
    4 : Message_Message.update,
    5 : Message_Message.remove,
    0 : Message_Message.notSet
  };
  static final $pb.BuilderInfo _i = $pb.BuilderInfo(_omitMessageNames ? '' : 'Message', package: const $pb.PackageName(_omitMessageNames ? '' : 'shrapnel.presets'), createEmptyInstance: create)
    ..oo(0, [1, 2, 3, 4, 5])
    ..aOM<Initialise>(1, _omitFieldNames ? '' : 'initialise', subBuilder: Initialise.create)
    ..aOM<Notify>(2, _omitFieldNames ? '' : 'notify', subBuilder: Notify.create)
    ..aOM<Create>(3, _omitFieldNames ? '' : 'create', subBuilder: Create.create)
    ..aOM<Update>(4, _omitFieldNames ? '' : 'update', subBuilder: Update.create)
    ..aOM<Remove>(5, _omitFieldNames ? '' : 'remove', subBuilder: Remove.create)
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
  Initialise get initialise => $_getN(0);
  @$pb.TagNumber(1)
  set initialise(Initialise v) { setField(1, v); }
  @$pb.TagNumber(1)
  $core.bool hasInitialise() => $_has(0);
  @$pb.TagNumber(1)
  void clearInitialise() => clearField(1);
  @$pb.TagNumber(1)
  Initialise ensureInitialise() => $_ensure(0);

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
  Create get create_3 => $_getN(2);
  @$pb.TagNumber(3)
  set create_3(Create v) { setField(3, v); }
  @$pb.TagNumber(3)
  $core.bool hasCreate_3() => $_has(2);
  @$pb.TagNumber(3)
  void clearCreate_3() => clearField(3);
  @$pb.TagNumber(3)
  Create ensureCreate_3() => $_ensure(2);

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
}


const _omitFieldNames = $core.bool.fromEnvironment('protobuf.omit_field_names');
const _omitMessageNames = $core.bool.fromEnvironment('protobuf.omit_message_names');
