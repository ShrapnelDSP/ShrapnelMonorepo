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
  group('Get MIDI mapping: ', () {
    test('success', () async {
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
            "123": { "midi_channel": 1, "cc_number": 2, "parameter_id": "gain" }
          }
        }
        ''',
      ) as Map<String, dynamic>;

      final requestJson = json.decode(
        '''
        {
          "messageType": "MidiMap::get::request"
        }
        ''',
      ) as Map<String, dynamic>;

      when(fakeWebsocket.send(requestJson))
          .thenAnswer((_) => controller.add(responseJson));
      when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

      expect(listenerCount, 0);
      expect(uut.mappings, isEmpty);

      await uut.getMapping();
      verify(fakeWebsocket.send(requestJson));
      expect(uut.mappings, {
        '123':
            const MidiMapping(midiChannel: 1, ccNumber: 2, parameterId: 'gain'),
      });

      await pumpEventQueue();
      expect(listenerCount, 0);

      await controller.close();
    });

    test('timeout', () async {
      var listenerCount = 0;
      final controller = StreamController<Map<String, dynamic>>.broadcast(
        onListen: () => listenerCount++,
        onCancel: () => listenerCount--,
      );

      final fakeWebsocket = MockJsonWebsocket();
      final uut = MidiMappingService(websocket: fakeWebsocket);

      when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

      expect(listenerCount, 0);
      expect(uut.mappings, isEmpty);

      await uut.getMapping();
      expect(uut.mappings, isEmpty);

      // wait for timeout
      await Future<void>.delayed(const Duration(seconds: 1));

      expect(listenerCount, 0);

      await controller.close();
    });
  });

  group('Create MIDI mapping: ', () {
    test('success', () async {
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
          "messageType": "MidiMap::create::response",
          "mapping" : {
            "123": { "midi_channel": 1, "cc_number": 2, "parameter_id": "gain" }
          }
        }
        ''',
      ) as Map<String, dynamic>;

      final requestJson = json.decode(
        '''
        {
          "messageType": "MidiMap::create::request",
          "mapping" : {
            "123": { "midi_channel": 1, "cc_number": 2, "parameter_id": "gain" }
          }
        }
        ''',
      ) as Map<String, dynamic>;

      when(fakeWebsocket.send(requestJson))
          .thenAnswer((_) => controller.add(responseJson));
      when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

      expect(listenerCount, 0);
      expect(uut.mappings, isEmpty);

      await uut.createMapping(
        const MidiMappingEntry(
          id: '123',
          mapping: MidiMapping(
            midiChannel: 1,
            ccNumber: 2,
            parameterId: 'gain',
          ),
        ),
      );
      verify(fakeWebsocket.send(requestJson));
      expect(uut.mappings, hasLength(1));

      await pumpEventQueue();
      expect(listenerCount, 0);

      await controller.close();
    });

    test('timeout', () async {
      var listenerCount = 0;
      final controller = StreamController<Map<String, dynamic>>.broadcast(
        onListen: () => listenerCount++,
        onCancel: () => listenerCount--,
      );

      final fakeWebsocket = MockJsonWebsocket();
      final uut = MidiMappingService(websocket: fakeWebsocket);

      final requestJson = json.decode(
        '''
        {
          "messageType": "MidiMap::create::request",
          "123": { "midi_channel": 1, "cc_number": 2, "parameter_id": "gain" }
        }
        ''',
      ) as Map<String, dynamic>;

      when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

      expect(listenerCount, 0);
      expect(uut.mappings, isEmpty);

      final result = uut.createMapping(
        const MidiMappingEntry(
          id: '123',
          mapping: MidiMapping(
            midiChannel: 1,
            ccNumber: 2,
            parameterId: 'gain',
          ),
        ),
      );
      expect(uut.mappings, hasLength(1));
      await result;

      await pumpEventQueue();
      expect(uut.mappings, isEmpty);
      expect(listenerCount, 0);

      await controller.close();
    });
  });
}
