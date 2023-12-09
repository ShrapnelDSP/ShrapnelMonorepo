import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/presets/model/presets_client.dart';
import 'package:shrapnel/presets/model/selected_preset_client.dart';

void main() {
  test('PresetsMessage.notify from JSON', () {
    final reference = PresetsMessage.notify(
      42,
      PresetData(
        name: 'test',
        parameters: PresetParametersData(
          ampGain: 0.001,
          ampChannel: 0.002,
          bass: 0.003,
          middle: 0.004,
          treble: 0.005,
          contour: 0.006,
          volume: 0.007,
          noiseGateThreshold: 0.008,
          noiseGateHysteresis: 0.009,
          noiseGateAttack: 0.010,
          noiseGateHold: 0.011,
          noiseGateRelease: 0.012,
          noiseGateBypass: 0.013,
          chorusRate: 0.014,
          chorusDepth: 0.015,
          chorusMix: 0.016,
          chorusBypass: 0.017,
          wahPosition: 0.018,
          wahVocal: 0.019,
          wahBypass: 0.020,
        ),
      ),
    );

    final jsonMessage = json.decode('''
    {
        "messageType": "Presets::notify",
        "id": 42,
        "preset": {
        "name": "test",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
        }
    }''') as Map<String, dynamic>;

    expect(PresetsMessage.fromJson(jsonMessage), reference);
  });

  test('PresetsMessage.initialise to JSON', () {
    final message = PresetsMessage.initialise();

    final reference = json.decode('''
    {
     "messageType": "Presets::initialise"
    }
    ''') as Map<String, dynamic>;

    expect(message.toJson(), reference);
  });

  test('PresetsMessage.create to JSON', () {
    final message = PresetsMessage.create(
      PresetData(
        name: 'test',
        parameters: PresetParametersData(
          ampGain: 0.001,
          ampChannel: 0.002,
          bass: 0.003,
          middle: 0.004,
          treble: 0.005,
          contour: 0.006,
          volume: 0.007,
          noiseGateThreshold: 0.008,
          noiseGateHysteresis: 0.009,
          noiseGateAttack: 0.010,
          noiseGateHold: 0.011,
          noiseGateRelease: 0.012,
          noiseGateBypass: 0.013,
          chorusRate: 0.014,
          chorusDepth: 0.015,
          chorusMix: 0.016,
          chorusBypass: 0.017,
          wahPosition: 0.018,
          wahVocal: 0.019,
          wahBypass: 0.020,
        ),
      ),
    );

    final reference = json.decode('''
    {
      "messageType": "Presets::create",
      "preset": {
        "name": "test",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
      }
    }
    ''') as Map<String, dynamic>;

    expect(message.toJson(), reference);
  });

  test('PresetsMessage.update to JSON', () {
    final message = PresetsMessage.update(
      42,
      PresetData(
        name: 'test',
        parameters: PresetParametersData(
          ampGain: 0.001,
          ampChannel: 0.002,
          bass: 0.003,
          middle: 0.004,
          treble: 0.005,
          contour: 0.006,
          volume: 0.007,
          noiseGateThreshold: 0.008,
          noiseGateHysteresis: 0.009,
          noiseGateAttack: 0.010,
          noiseGateHold: 0.011,
          noiseGateRelease: 0.012,
          noiseGateBypass: 0.013,
          chorusRate: 0.014,
          chorusDepth: 0.015,
          chorusMix: 0.016,
          chorusBypass: 0.017,
          wahPosition: 0.018,
          wahVocal: 0.019,
          wahBypass: 0.020,
        ),
      ),
    );

    final reference = json.decode('''
    {
      "messageType": "Presets::update",
      "id": 42,
      "preset": {
        "name": "test",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
      }
    }
    ''') as Map<String, dynamic>;

    expect(message.toJson(), reference);
  });

  test('SelectedPresetMessage.notify from JSON', () {
    final reference = SelectedPresetMessage.notify(selectedPreset: 42);

    final jsonMessage = json.decode('''
    {
      "messageType": "SelectedPreset::notify",
      "selectedPreset": 42
    }
    ''') as Map<String, dynamic>;

    expect(SelectedPresetMessage.fromJson(jsonMessage), reference);
  });

  test('SelectedPresetMessage.read from JSON', () {
    final message = SelectedPresetMessage.read();

    final reference = json.decode('''
    {
      "messageType": "SelectedPreset::read"
    }
    ''') as Map<String, dynamic>;

    expect(message.toJson(), reference);
  });

  test('SelectedPresetMessage.write from JSON', () {
    final message = SelectedPresetMessage.write(selectedPreset: 42);

    final reference = json.decode('''
    {
      "messageType": "SelectedPreset::write",
      "selectedPreset": 42
    }
    ''') as Map<String, dynamic>;

    expect(message.toJson(), reference);
  });
}
