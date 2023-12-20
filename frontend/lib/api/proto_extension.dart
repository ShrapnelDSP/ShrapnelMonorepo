import 'package:uuid/uuid.dart';

import '../audio_events.dart';
import '../midi_mapping/model/models.dart';
import '../parameter.dart';
import '../presets/model/presets_client.dart';
import '../presets/model/selected_preset_client.dart';
import 'api_websocket.dart';
import 'generated/audio_events.pb.dart' as audio_events_pb;
import 'generated/cmd_handling.pb.dart' as cmd_handling_pb;
import 'generated/midi_mapping.pb.dart' as midi_mapping_pb;
import 'generated/presets.pb.dart' as presets_pb;
import 'generated/selected_preset.pb.dart' as selected_preset_pb;
import 'generated/shrapnel.pb.dart' as shrapnel_pb;
import 'generated/uuid.pb.dart' as uuid_pb;

class ProtoException implements Exception {}

extension ApiMessageProtoEx on ApiMessage {
  static ApiMessage fromProto(shrapnel_pb.Message proto) {
    return switch (proto.whichMessage()) {
      shrapnel_pb.Message_Message.audioEvent => ApiMessage.audioEvent(
          message: AudioEventProtoEx.fromProto(proto.audioEvent),
        ),
      shrapnel_pb.Message_Message.command => ApiMessage.parameterInput(
          message: ParameterServiceInputMessageProtoEx.fromProto(proto.command),
        ),
      shrapnel_pb.Message_Message.midiMapping => ApiMessage.midiMapping(
          message: MidiProtoEx.fromProto(proto.midiMapping),
        ),
      shrapnel_pb.Message_Message.preset => ApiMessage.presets(
          message: PresetsMessageProtoEx.fromProto(proto.preset),
        ),
      shrapnel_pb.Message_Message.selectedPreset => ApiMessage.selectedPreset(
          message: SelectedPresetMessageProtoEx.fromProto(proto.selectedPreset),
        ),
      shrapnel_pb.Message_Message.notSet => throw ProtoException(),
    };
  }

  shrapnel_pb.Message toProto() {
    return switch (this) {
      ApiMessageAudioEvent(:final message) =>
        shrapnel_pb.Message(audioEvent: message.toProto()),
      ApiMessageParameterInput() => throw ProtoException(),
      ApiMessageParameterOutput(:final message) => shrapnel_pb.Message(
          command: message.toProto(),
        ),
      ApiMessageMidiMapping(:final message) =>
        shrapnel_pb.Message(midiMapping: message.toProto()),
      ApiMessagePresets(:final message) =>
        shrapnel_pb.Message(preset: message.toProto()),
      ApiMessageSelectedPreset(:final message) => shrapnel_pb.Message(
          selectedPreset: message.toProto(),
        ),
    };
  }
}

extension AudioEventProtoEx on AudioEventMessage {
  static AudioEventMessage fromProto(audio_events_pb.Message proto) {
    return switch (proto.whichEvent()) {
      audio_events_pb.Message_Event.inputClipped =>
        AudioEventInputClippedProtoEx.fromProto(proto.inputClipped),
      audio_events_pb.Message_Event.outputClipped =>
        AudioEventOutputClippedProtoEx.fromProto(proto.outputClipped),
      audio_events_pb.Message_Event.notSet => throw ProtoException(),
    };
  }

  audio_events_pb.Message toProto() {
    return switch (this) {
      AudioEventMessageInputClipped() =>
        audio_events_pb.Message(inputClipped: audio_events_pb.InputClipped()),
      AudioEventMessageOutputClipped() =>
        audio_events_pb.Message(outputClipped: audio_events_pb.OutputClipped()),
    };
  }
}

extension AudioEventInputClippedProtoEx on AudioEventMessageInputClipped {
  static AudioEventMessage fromProto(audio_events_pb.InputClipped proto) {
    return AudioEventMessage.inputClipped();
  }
}

extension AudioEventOutputClippedProtoEx on AudioEventMessageOutputClipped {
  static AudioEventMessage fromProto(audio_events_pb.OutputClipped proto) {
    return AudioEventMessage.outputClipped();
  }
}

extension ParameterServiceInputMessageProtoEx on ParameterServiceInputMessage {
  static ParameterServiceInputMessage fromProto(cmd_handling_pb.Message proto) {
    return switch (proto.whichMessage()) {
      cmd_handling_pb.Message_Message.update =>
        ParameterServiceInputMessageUpdateProtoEx.fromProto(proto.update),
      cmd_handling_pb.Message_Message.initialise => throw ProtoException(),
      cmd_handling_pb.Message_Message.notSet => throw ProtoException(),
    };
  }
}

extension ParameterServiceOutputMessageProtoEx
    on ParameterServiceOutputMessage {
  cmd_handling_pb.Message toProto() {
    return switch (this) {
      ParameterServiceOutputMessageRequestInitialisation() =>
        cmd_handling_pb.Message(initialise: cmd_handling_pb.Initialise()),
      ParameterServiceOutputMessageParameterUpdate(:final id, :final value) =>
        cmd_handling_pb.Message(
          update: cmd_handling_pb.Update(
            id: id,
            value: value,
          ),
        ),
    };
  }
}

extension ParameterServiceInputMessageUpdateProtoEx
    on ParameterServiceInputMessageParameterUpdate {
  static ParameterServiceInputMessage fromProto(cmd_handling_pb.Update proto) {
    return ParameterServiceInputMessage.parameterUpdate(
      id: proto.id,
      value: proto.value,
    );
  }
}

extension MidiProtoEx on MidiApiMessage {
  static MidiApiMessage fromProto(midi_mapping_pb.Message proto) {
    return switch (proto.whichMessage()) {
      midi_mapping_pb.Message_Message.getRequest => throw ProtoException(),
      midi_mapping_pb.Message_Message.getResponse =>
        MidiGetResponseProtoEx.fromProto(proto.getResponse),
      midi_mapping_pb.Message_Message.createRequest => throw ProtoException(),
      midi_mapping_pb.Message_Message.createResponse =>
        MidiCreateResponseProtoEx.fromProto(proto.createResponse),
      midi_mapping_pb.Message_Message.update => throw ProtoException(),
      midi_mapping_pb.Message_Message.remove => throw ProtoException(),
      midi_mapping_pb.Message_Message.messageReceived =>
        MidiMessageReceivedProtoEx.fromProto(proto.messageReceived),
      midi_mapping_pb.Message_Message.notSet => throw ProtoException(),
    };
  }

  midi_mapping_pb.Message toProto() {
    return switch (this) {
      MidiGetRequest() =>
        midi_mapping_pb.Message(getRequest: midi_mapping_pb.GetRequest()),
      MidiGetResponse() => throw ProtoException(),
      MidiCreateRequest(:final mapping) => midi_mapping_pb.Message(
          createRequest: midi_mapping_pb.CreateRequest(
            mapping: midi_mapping_pb.MappingRecord(
              id: UuidValue(mapping.id).toProto(),
              mapping: mapping.mapping.toProto(),
            ),
          ),
        ),
      MidiCreateResponse() => throw ProtoException(),
      MidiUpdate(:final mapping) => midi_mapping_pb.Message(
          update: midi_mapping_pb.Update(
            mapping: midi_mapping_pb.MappingRecord(
              id: UuidValue(mapping.id).toProto(),
              mapping: mapping.mapping.toProto(),
            ),
          ),
        ),
      MidiRemove(:final id) => midi_mapping_pb.Message(
          remove: midi_mapping_pb.Remove(
            id: UuidValue(id).toProto(),
          ),
        ),
      MidiMessageReceived() => throw ProtoException(),
    };
  }
}

extension MidiGetResponseProtoEx on MidiGetResponse {
  static MidiApiMessage fromProto(midi_mapping_pb.GetResponse proto) {
    return MidiApiMessage.getResponse(
      mappings: MidiMappingsProtoEx.fromProto(proto.mappings),
    );
  }
}

extension MidiMappingsProtoEx on Map<String, MidiMapping> {
  static Map<String, MidiMapping> fromProto(midi_mapping_pb.MappingList proto) {
    final out = <String, MidiMapping>{};
    for (final protoEntry in proto.mappings) {
      final entry = MidiMappingEntryProtoEx.fromProto(protoEntry);
      out[entry.id] = entry.mapping;
    }
    return out;
  }
}

extension MidiMappingEntryProtoEx on MidiMappingEntry {
  static MidiMappingEntry fromProto(midi_mapping_pb.MappingRecord proto) {
    return MidiMappingEntry(
      id: UuidValueProtoEx.fromProto(proto.id).uuid,
      mapping: MidiMappingProtoEx.fromProto(proto.mapping),
    );
  }
}

extension UuidValueProtoEx on UuidValue {
  static UuidValue fromProto(uuid_pb.Uuid proto) {
    return UuidValue.fromList(proto.value);
  }

  uuid_pb.Uuid toProto() {
    return uuid_pb.Uuid(value: toBytes());
  }
}

extension MidiMappingProtoEx on MidiMapping {
  static MidiMapping fromProto(midi_mapping_pb.Mapping proto) {
    return MidiMapping(
      midiChannel: proto.midiChannel,
      ccNumber: proto.ccNumber,
      parameterId: proto.parameterName,
      mode: MidiMappingModeProtoEx.fromProto(proto.mode),
    );
  }

  midi_mapping_pb.Mapping toProto() {
    return midi_mapping_pb.Mapping(
      midiChannel: midiChannel,
      ccNumber: ccNumber,
      mode: mode.toProto(),
      parameterName: parameterId,
    );
  }
}

extension MidiMappingModeProtoEx on MidiMappingMode {
  static MidiMappingMode fromProto(midi_mapping_pb.Mapping_Mode proto) {
    return switch (proto) {
      midi_mapping_pb.Mapping_Mode.parameter => MidiMappingMode.parameter,
      midi_mapping_pb.Mapping_Mode.toggle => MidiMappingMode.toggle,
      _ => throw ProtoException(),
    };
  }

  midi_mapping_pb.Mapping_Mode toProto() {
    return switch (this) {
      MidiMappingMode.toggle => midi_mapping_pb.Mapping_Mode.parameter,
      MidiMappingMode.parameter => midi_mapping_pb.Mapping_Mode.parameter,
    };
  }
}

extension MidiCreateResponseProtoEx on MidiCreateResponse {
  static MidiCreateResponse fromProto(midi_mapping_pb.CreateResponse proto) {
    return MidiCreateResponse(
      mapping: MidiMappingEntryProtoEx.fromProto(proto.mapping),
    );
  }
}

extension MidiMessageReceivedProtoEx on MidiMessageReceived {
  static MidiMessageReceived fromProto(midi_mapping_pb.MessageReceived proto) {
    return MidiMessageReceived(
      message: MidiMessageProtoEx.fromProto(proto.receivedMessage),
    );
  }
}

extension MidiMessageProtoEx on MidiMessage {
  static MidiMessage fromProto(midi_mapping_pb.MidiMessage proto) {
    return switch (proto.whichParameters()) {
      midi_mapping_pb.MidiMessage_Parameters.noteOn => MidiMessage.noteOn(
          channel: proto.channel,
          note: proto.noteOn.note,
          velocity: proto.noteOn.velocity,
        ),
      midi_mapping_pb.MidiMessage_Parameters.noteOff => MidiMessage.noteOff(
          channel: proto.channel,
          note: proto.noteOff.note,
          velocity: proto.noteOff.velocity,
        ),
      midi_mapping_pb.MidiMessage_Parameters.controlChange =>
        MidiMessage.controlChange(
          channel: proto.channel,
          control: proto.controlChange.control,
          value: proto.controlChange.value,
        ),
      midi_mapping_pb.MidiMessage_Parameters.programChange =>
        MidiMessage.programChange(
          channel: proto.channel,
          number: proto.programChange.number,
        ),
      midi_mapping_pb.MidiMessage_Parameters.notSet => throw ProtoException(),
    };
  }
}

extension PresetsMessageProtoEx on PresetsMessage {
  static PresetsMessage fromProto(presets_pb.Message proto) {
    return switch (proto.whichMessage()) {
      presets_pb.Message_Message.initialise => PresetsMessage.initialise(),
      presets_pb.Message_Message.notify => PresetsMessage.notify(
          proto.notify.preset.id,
          PresetDataProtoEx.fromProto(proto.notify.preset.preset),
        ),
      presets_pb.Message_Message.create_3 => PresetsMessage.create(
          PresetDataProtoEx.fromProto(proto.create_3.preset),
        ),
      presets_pb.Message_Message.update => PresetsMessage.update(
          proto.update.preset.id,
          PresetDataProtoEx.fromProto(proto.update.preset.preset),
        ),
      presets_pb.Message_Message.remove =>
        PresetsMessage.delete(proto.remove.id),
      presets_pb.Message_Message.notSet => throw ProtoException(),
    };
  }

  presets_pb.Message toProto() {
    return switch (this) {
      PresetsMessageInitialise() =>
        presets_pb.Message(initialise: presets_pb.Initialise()),
      PresetsMessageNotify(:final id, :final preset) => presets_pb.Message(
          notify: presets_pb.Notify(
            preset: presets_pb.PresetRecord(
              id: id,
              preset: preset.toProto(),
            ),
          ),
        ),
      PresetsMessageCreate(:final preset) => presets_pb.Message(
          create_3: presets_pb.Create(
            preset: preset.toProto(),
          ),
        ),
      PresetsMessageUpdate(:final id, :final preset) => presets_pb.Message(
          update: presets_pb.Update(
            preset: presets_pb.PresetRecord(
              id: id,
              preset: preset.toProto(),
            ),
          ),
        ),
      PresetsMessageDelete(:final id) =>
        presets_pb.Message(remove: presets_pb.Remove(id: id)),
    };
  }
}

extension PresetDataProtoEx on PresetData {
  static PresetData fromProto(presets_pb.Preset proto) {
    return PresetData(
      name: proto.name,
      parameters: PresetParametersDataProtoEx.fromProto(proto.parameters),
    );
  }

  presets_pb.Preset toProto() {
    return presets_pb.Preset(
      name: name,
      parameters: parameters.toProto(),
    );
  }
}

extension PresetParametersDataProtoEx on PresetParametersData {
  static PresetParametersData fromProto(presets_pb.PresetParameters proto) {
    return PresetParametersData(
      ampGain: proto.ampGain / 1000,
      ampChannel: proto.ampChannel / 1000,
      bass: proto.bass / 1000,
      middle: proto.middle / 1000,
      treble: proto.treble / 1000,
      contour: proto.contour / 1000,
      volume: proto.volume / 1000,
      noiseGateThreshold: proto.noiseGateThreshold / 1000,
      noiseGateHysteresis: proto.noiseGateHysteresis / 1000,
      noiseGateAttack: proto.noiseGateAttack / 1000,
      noiseGateHold: proto.noiseGateHold / 1000,
      noiseGateRelease: proto.noiseGateRelease / 1000,
      noiseGateBypass: proto.noiseGateBypass / 1000,
      chorusRate: proto.chorusRate / 1000,
      chorusDepth: proto.chorusDepth / 1000,
      chorusMix: proto.chorusMix / 1000,
      chorusBypass: proto.chorusBypass / 1000,
      wahPosition: proto.wahPosition / 1000,
      wahVocal: proto.wahVocal / 1000,
      wahBypass: proto.wahBypass / 1000,
    );
  }

  presets_pb.PresetParameters toProto() {
    return presets_pb.PresetParameters(
      ampGain: (ampGain * 1000).round(),
      ampChannel: (ampChannel * 1000).round(),
      bass: (bass * 1000).round(),
      middle: (middle * 1000).round(),
      treble: (treble * 1000).round(),
      contour: (contour * 1000).round(),
      volume: (volume * 1000).round(),
      noiseGateThreshold: (noiseGateThreshold * 1000).round(),
      noiseGateHysteresis: (noiseGateHysteresis * 1000).round(),
      noiseGateAttack: (noiseGateAttack * 1000).round(),
      noiseGateHold: (noiseGateHold * 1000).round(),
      noiseGateRelease: (noiseGateRelease * 1000).round(),
      noiseGateBypass: (noiseGateBypass * 1000).round(),
      chorusRate: (chorusRate * 1000).round(),
      chorusDepth: (chorusDepth * 1000).round(),
      chorusMix: (chorusMix * 1000).round(),
      chorusBypass: (chorusBypass * 1000).round(),
      wahPosition: (wahPosition * 1000).round(),
      wahVocal: (wahVocal * 1000).round(),
      wahBypass: (wahBypass * 1000).round(),
    );
  }
}

extension SelectedPresetMessageProtoEx on SelectedPresetMessage {
  static SelectedPresetMessage fromProto(selected_preset_pb.Message proto) {
    return switch (proto.whichMessage()) {
      selected_preset_pb.Message_Message.read => SelectedPresetMessage.read(),
      selected_preset_pb.Message_Message.notify =>
        SelectedPresetMessage.notify(selectedPreset: proto.notify.id),
      selected_preset_pb.Message_Message.write =>
        SelectedPresetMessage.write(selectedPreset: proto.write.id),
      selected_preset_pb.Message_Message.notSet => throw ProtoException(),
    };
  }

  selected_preset_pb.Message toProto() {
    return switch (this) {
      ReadSelectedPresetMessage() =>
        selected_preset_pb.Message(read: selected_preset_pb.Read()),
      NotifySelectedPresetMessage(:final selectedPreset) =>
        selected_preset_pb.Message(
          notify: selected_preset_pb.Notify(id: selectedPreset),
        ),
      WriteSelectedPresetMessage(:final selectedPreset) =>
        selected_preset_pb.Message(
          write: selected_preset_pb.Write(id: selectedPreset),
        ),
    };
  }
}
