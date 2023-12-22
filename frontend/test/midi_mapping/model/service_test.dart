/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

import 'dart:async';
import 'dart:collection';

import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/core/message_transport.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';
import 'package:shrapnel/midi_mapping/model/service.dart';
import 'service_test.mocks.dart';

@GenerateMocks([MessageTransport])
void main() {
  group('Get MIDI mapping: ', () {
    test('success', () async {
      var listenerCount = 0;
      final controller = StreamController<MidiApiMessage>.broadcast(
        onListen: () => listenerCount++,
        onCancel: () => listenerCount--,
      );

      final outputMessages = Queue<MidiApiMessage>();
      final sinkController = StreamController<MidiApiMessage>();

      final fakeWebsocket =
          MockMessageTransport<MidiApiMessage, MidiApiMessage>();
      when(fakeWebsocket.connectionStream)
          .thenAnswer((_) => Stream.fromIterable([]));
      when(fakeWebsocket.isAlive).thenReturn(false);

      const response = MidiApiMessage.update(
        mapping: MidiMappingEntry(
          id: '123',
          mapping: MidiMapping(
            midiChannel: 1,
            ccNumber: 2,
            parameterId: 'gain',
            mode: MidiMappingMode.parameter,
          ),
        ),
      );

      const request = MidiApiMessage.getRequest();

      sinkController.stream.listen((event) {
        outputMessages.add(event);

        if (event == request) {
          controller.add(response);
        }
      });

      when(fakeWebsocket.sink).thenAnswer((_) => sinkController);
      when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

      final uut = MidiMappingService(websocket: fakeWebsocket);

      expect(listenerCount, 1);
      expect(uut.mappings, isEmpty);

      await uut.getMapping();
      await pumpEventQueue();

      expect(outputMessages.removeLast(), request);
      expect(outputMessages, isEmpty);
      expect(uut.mappings, {
        '123': const MidiMapping(
          midiChannel: 1,
          ccNumber: 2,
          mode: MidiMappingMode.parameter,
          parameterId: 'gain',
        ),
      });

      uut.dispose();

      expect(listenerCount, 0);

      await controller.close();
    });
  });

  group('Create MIDI mapping: ', () {
    test('success', () async {
      var listenerCount = 0;
      final controller = StreamController<MidiApiMessage>.broadcast(
        onListen: () => listenerCount++,
        onCancel: () => listenerCount--,
      );

      final outputMessages = Queue<MidiApiMessage>();
      final sinkController = StreamController<MidiApiMessage>();

      final fakeWebsocket =
          MockMessageTransport<MidiApiMessage, MidiApiMessage>();
      when(fakeWebsocket.connectionStream)
          .thenAnswer((_) => Stream.fromIterable([]));
      when(fakeWebsocket.isAlive).thenReturn(false);

      const response = MidiApiMessage.createResponse(
        mapping: MidiMappingEntry(
          id: '123',
          mapping: MidiMapping(
            midiChannel: 1,
            ccNumber: 2,
            mode: MidiMappingMode.parameter,
            parameterId: 'gain',
          ),
        ),
      );

      const request = MidiApiMessage.createRequest(
        mapping: MidiMappingEntry(
          id: '123',
          mapping: MidiMapping(
            midiChannel: 1,
            ccNumber: 2,
            mode: MidiMappingMode.parameter,
            parameterId: 'gain',
          ),
        ),
      );

      sinkController.stream.listen((event) {
        outputMessages.add(event);

        if (event == request) {
          controller.add(response);
        }
      });
      when(fakeWebsocket.sink).thenAnswer((_) => sinkController);
      when(fakeWebsocket.stream).thenAnswer((_) => controller.stream);

      final uut = MidiMappingService(websocket: fakeWebsocket);

      expect(listenerCount, 1);
      expect(uut.mappings, isEmpty);

      await uut.createMapping(
        const MidiMappingEntry(
          id: '123',
          mapping: MidiMapping(
            midiChannel: 1,
            ccNumber: 2,
            mode: MidiMappingMode.parameter,
            parameterId: 'gain',
          ),
        ),
      );
      await pumpEventQueue();

      expect(outputMessages.removeLast(), request);
      expect(outputMessages, isEmpty);
      expect(uut.mappings, hasLength(1));

      uut.dispose();
      expect(listenerCount, 0);

      await controller.close();
    });
  });
}
