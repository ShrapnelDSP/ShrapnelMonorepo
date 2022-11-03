import 'dart:async';
import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/json_websocket.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';
import 'package:shrapnel/midi_mapping/model/service.dart';
import 'service_test.mocks.dart';

@GenerateMocks([JsonWebsocket])
void main() {
  test('Get MIDI mapping', () async {
    var listenerCount = 0;
    final controller = StreamController<Map<String, dynamic>>.broadcast(
      onListen: () => listenerCount++,
      onCancel: () => listenerCount--,
    );

    final fakeWebsocket = MockJsonWebsocket();
    final uut = MidiMappingService(websocket: fakeWebsocket);

    final responseJson = json.decode(
      '''
      {
        "messageType": "MidiMap::get::response",
        "mappings": {
          "123": { "midi_channel": 0, "cc_number": 2, "parameter_id": "gain" }
        }
      }
      ''',
    ) as Map<String, dynamic>;

    final responseMapping = {
      '123':
          const MidiMapping(midiChannel: 0, ccNumber: 2, parameterId: 'gain'),
    };

    final requestJson = json.decode(
      '''
      {
        "messageType": "MidiMap::get::request"
      }
      ''',
    ) as Map<String, dynamic>;

    when(fakeWebsocket.send(requestJson)).thenReturn(null);
    when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

    expect(listenerCount, 0);
    expect(uut.mappings, isEmpty);

    uut.getMapping();
    controller.add(responseJson);
    await Future<void>.delayed(const Duration(seconds: 1));
    expect(uut.mappings, responseMapping);

    // delay required for the event loop to process cancelling the subscription
    await Future<void>.delayed(const Duration(seconds: 1));
    expect(listenerCount, 0);

    await controller.close();
  });
}
