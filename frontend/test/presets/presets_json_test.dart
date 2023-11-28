import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/presets/model/presets_client.dart';
import 'package:uuid/uuid.dart';

void main() {
  test('PresetsMessage.notify to JSON', () {
    final reference = PresetsMessage.notify(
      PresetData(
        id: UuidValue("00000000-0000-0000-0000-000000000000"),
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
        "preset": {
        "id": "00000000-0000-0000-0000-000000000000",
        "name": "test",
        "parameters": "CAEQAhgDIAQoBTAGOAdACEgJUApYC2AMaA1wDngPgAEQiAERkAESmAEToAEU"
        }
    }''') as Map<String, dynamic>;

    expect(PresetsMessage.fromJson(jsonMessage), reference);
  });
}
