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

  static shrapnel_pb.Message toProto(ApiMessage message) {
    return switch (message) {
      ApiMessageAudioEvent(:final message) =>
        shrapnel_pb.Message(audioEvent: AudioEventProtoEx.toProto(message)),
      ApiMessageParameterInput() => throw ProtoException(),
      ApiMessageParameterOutput(:final message) => shrapnel_pb.Message(
          command: ParameterServiceOutputMessageProtoEx.toProto(message),
        ),
      ApiMessageMidiMapping(:final message) =>
        shrapnel_pb.Message(midiMapping: MidiProtoEx.toProto(message)),
      ApiMessagePresets(:final message) =>
        shrapnel_pb.Message(preset: PresetsMessageProtoEx.toProto(message)),
      ApiMessageSelectedPreset(:final message) => shrapnel_pb.Message(
          selectedPreset: SelectedPresetMessageProtoEx.toProto(message),
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

  static audio_events_pb.Message toProto(AudioEventMessage message) {
    return switch (message) {
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
  static cmd_handling_pb.Message toProto(
    ParameterServiceOutputMessage message,
  ) {
    return switch (message) {
      ParameterServiceOutputMessageRequestInitialisation() =>
        cmd_handling_pb.Message(initialise: cmd_handling_pb.Initialise()),
      ParameterServiceOutputMessageParameterUpdate() => cmd_handling_pb.Message(
          update: cmd_handling_pb.Update(
            id: message.id,
            value: message.value,
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

  static midi_mapping_pb.Message toProto(MidiApiMessage message) {
    return switch (message) {
      MidiGetRequest() =>
        midi_mapping_pb.Message(getRequest: midi_mapping_pb.GetRequest()),
      MidiGetResponse() => throw ProtoException(),
      MidiCreateRequest() => midi_mapping_pb.Message(
          createRequest: midi_mapping_pb.CreateRequest(
            mapping: midi_mapping_pb.MappingRecord(
              id: UuidValueProtoEx.toProto(UuidValue(message.mapping.id)),
              mapping: MidiMappingProtoEx.toProto(
                message.mapping.mapping,
              ),
            ),
          ),
        ),
      MidiCreateResponse() => throw ProtoException(),
      MidiUpdate() => midi_mapping_pb.Message(
          update: midi_mapping_pb.Update(
            mapping: midi_mapping_pb.MappingRecord(
              id: UuidValueProtoEx.toProto(UuidValue(message.mapping.id)),
              mapping: MidiMappingProtoEx.toProto(
                message.mapping.mapping,
              ),
            ),
          ),
        ),
      MidiRemove() => midi_mapping_pb.Message(
          remove: midi_mapping_pb.Remove(
            id: UuidValueProtoEx.toProto(UuidValue(message.id)),
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

  static uuid_pb.Uuid toProto(UuidValue message) {
    return uuid_pb.Uuid(value: message.toBytes());
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

  static midi_mapping_pb.Mapping toProto(MidiMapping mapping) {
    return midi_mapping_pb.Mapping(
      midiChannel: mapping.midiChannel,
      ccNumber: mapping.ccNumber,
      mode: MidiMappingModeProtoEx.toProto(mapping.mode),
      parameterName: mapping.parameterId,
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

  static midi_mapping_pb.Mapping_Mode toProto(MidiMappingMode mode) {
    return switch (mode) {
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

  static presets_pb.Message toProto(PresetsMessage message) {
    return switch (message) {
      PresetsMessageInitialise() =>
        presets_pb.Message(initialise: presets_pb.Initialise()),
      PresetsMessageNotify() => presets_pb.Message(
          notify: presets_pb.Notify(
            preset: presets_pb.PresetRecord(
              id: message.id,
              preset: PresetDataProtoEx.toProto(message.preset),
            ),
          ),
        ),
      PresetsMessageCreate() => presets_pb.Message(
          create_3: presets_pb.Create(
            preset: PresetDataProtoEx.toProto(message.preset),
          ),
        ),
      PresetsMessageUpdate() => presets_pb.Message(
          update: presets_pb.Update(
            preset: presets_pb.PresetRecord(
              id: message.id,
              preset: PresetDataProtoEx.toProto(message.preset),
            ),
          ),
        ),
      PresetsMessageDelete() =>
        presets_pb.Message(remove: presets_pb.Remove(id: message.id)),
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

  static presets_pb.Preset toProto(PresetData preset) {
    return presets_pb.Preset(
      name: preset.name,
      parameters: PresetParametersDataProtoEx.toProto(preset.parameters),
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

  static presets_pb.PresetParameters toProto(PresetParametersData message) {
    return presets_pb.PresetParameters(
      ampGain: (message.ampGain * 1000).round(),
      ampChannel: (message.ampChannel * 1000).round(),
      bass: (message.bass * 1000).round(),
      middle: (message.middle * 1000).round(),
      treble: (message.treble * 1000).round(),
      contour: (message.contour * 1000).round(),
      volume: (message.volume * 1000).round(),
      noiseGateThreshold: (message.noiseGateThreshold * 1000).round(),
      noiseGateHysteresis: (message.noiseGateHysteresis * 1000).round(),
      noiseGateAttack: (message.noiseGateAttack * 1000).round(),
      noiseGateHold: (message.noiseGateHold * 1000).round(),
      noiseGateRelease: (message.noiseGateRelease * 1000).round(),
      noiseGateBypass: (message.noiseGateBypass * 1000).round(),
      chorusRate: (message.chorusRate * 1000).round(),
      chorusDepth: (message.chorusDepth * 1000).round(),
      chorusMix: (message.chorusMix * 1000).round(),
      chorusBypass: (message.chorusBypass * 1000).round(),
      wahPosition: (message.wahPosition * 1000).round(),
      wahVocal: (message.wahVocal * 1000).round(),
      wahBypass: (message.wahBypass * 1000).round(),
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

  static selected_preset_pb.Message toProto(SelectedPresetMessage message) {
    return switch (message) {
      ReadSelectedPresetMessage() =>
        selected_preset_pb.Message(read: selected_preset_pb.Read()),
      NotifySelectedPresetMessage() => selected_preset_pb.Message(
          notify: selected_preset_pb.Notify(id: message.selectedPreset),
        ),
      WriteSelectedPresetMessage() => selected_preset_pb.Message(
          write: selected_preset_pb.Write(id: message.selectedPreset),
        ),
    };
  }
}
