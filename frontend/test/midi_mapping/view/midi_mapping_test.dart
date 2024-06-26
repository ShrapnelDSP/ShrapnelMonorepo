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

import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/api/api_websocket.dart';
import 'package:shrapnel/audio_events.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';
import 'package:shrapnel/robust_websocket.dart';

import '../../home_page_object.dart';
import 'midi_mapping_test.mocks.dart';

// required for mocking using mockito, implement clause doesn't support inline
// function types or typedefs to function types
// ignore: one_member_abstracts
abstract class _ApiSendBase {
  void call(ApiMessage message);
}

class _MockApiSend extends Mock implements _ApiSendBase {}

@GenerateMocks([ApiWebsocket])
@GenerateNiceMocks(
  [MockSpec<RobustWebsocket>(), MockSpec<AudioClippingService>()],
)
void main() {
  testWidgets('Midi mapping can be created', (tester) async {
    final apiController = StreamController<ApiMessage>.broadcast();
    final websocket = MockRobustWebsocket();
    final api = MockApiWebsocket();
    final apiSinkController = StreamController<ApiMessage>();
    final apiSend = _MockApiSend();
    apiSinkController.stream.listen(apiSend.call);
    when(api.stream).thenAnswer((_) => apiController.stream);
    when(api.sink).thenReturn(apiSinkController);
    when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
    when(api.isAlive).thenReturn(true);

    final sut = App(
      websocket: websocket,
      apiWebsocket: api,
    );

    const getRequest =
        ApiMessage.midiMapping(message: MidiApiMessage.getRequest());

    await tester.pumpWidget(sut);

    final midiMappingPage = await HomePageObject(tester).openMidiMapping();

    expect(midiMappingPage.findPage(), findsOneWidget);
    expect(midiMappingPage.findMappingRows(), findsNothing);
    verify(apiSend(getRequest));

    const createRequest = ApiMessage.midiMapping(
      message: MidiApiMessage.createRequest(
        mapping: MidiMapping.parameter(
          midiChannel: 1,
          ccNumber: 2,
          parameterId: 'chorusDepth',
        ),
      ),
    );

    when(apiSend(createRequest)).thenAnswer(
      (_) {
        apiController.add(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.createResponse(
              mapping: MidiMappingEntry(
                id: 123,
                mapping: MidiMapping.parameter(
                  midiChannel: 1,
                  ccNumber: 2,
                  parameterId: 'chorusDepth',
                ),
              ),
            ),
          ),
        );
      },
    );

    final midiMappingCreatePage = await midiMappingPage.openCreateDialog();
    await midiMappingCreatePage.selectMidiChannel(1);
    await midiMappingCreatePage.selectCcNumber(2);
    await midiMappingCreatePage.selectMode(MidiMappingMode.parameter);
    // XXX: There is a bug in flutter where the DropdownButton's popup menu is
    //      unreliable during tests: https://github.com/flutter/flutter/issues/82908
    //
    // Pick an arbitrary parameter here. The only criteria for selection is that
    // it actually works during the test. This is more likely if something is
    // picked from the top of the list. When changing the parameter name, make
    // sure that the parameter ID in the stub is also updated.
    await midiMappingCreatePage.selectParameter('Chorus: DEPTH');
    await midiMappingCreatePage.submitCreateDialog();

    verify(apiSend(createRequest));

    // Expect new mapping visible in UI
    // TODO check the correct value is visible in all dropdowns
    expect(midiMappingPage.findMappingRows(), findsOneWidget);

    await apiController.close();
  });

  testWidgets(
    'Midi mapping create rolls back on error',
    (tester) async {
      final apiController = StreamController<ApiMessage>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockApiWebsocket();
      final apiSinkController = StreamController<ApiMessage>();
      final apiSend = _MockApiSend();
      apiSinkController.stream.listen(apiSend.call);
      when(api.stream).thenAnswer((_) => apiController.stream);
      when(api.sink).thenReturn(apiSinkController);
      when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
      when(api.isAlive).thenReturn(true);

      final sut = App(
        websocket: websocket,
        apiWebsocket: api,
      );

      await tester.pumpWidget(sut);

      final midiMappingPage = await HomePageObject(tester).openMidiMapping();

      expect(midiMappingPage.findPage(), findsOneWidget);
      expect(midiMappingPage.findMappingRows(), findsNothing);

      const createRequest = ApiMessage.midiMapping(
        message: MidiApiMessage.createRequest(
          mapping: MidiMapping.parameter(
            midiChannel: 1,
            ccNumber: 2,
            parameterId: 'chorusDepth',
          ),
        ),
      );

      final midiMappingCreatePage = await midiMappingPage.openCreateDialog();
      await midiMappingCreatePage.selectMidiChannel(1);
      await midiMappingCreatePage.selectCcNumber(2);
      await midiMappingCreatePage.selectMode(MidiMappingMode.parameter);
      // XXX: There is a bug in flutter where the DropdownButton's popup menu is
      //      unreliable during tests: https://github.com/flutter/flutter/issues/82908
      //
      // Pick an arbitrary parameter here. The only criteria for selection is that
      // it actually works during the test. This is more likely if something is
      // picked from the top of the list. When changing the parameter name, make
      // sure that the parameter ID in the stub is also updated.
      await midiMappingCreatePage.selectParameter('Chorus: DEPTH');
      await midiMappingCreatePage.submitCreateDialog();

      verify(apiSend(createRequest));

      // Expect new mapping visible in UI
      expect(midiMappingPage.findMappingRows(), findsOneWidget);

      await tester.pump(const Duration(seconds: 1));

      expect(
        midiMappingPage.findMappingRows(),
        findsNothing,
        reason: 'UI was not rolled back after create request timeout',
      );

      await apiController.close();
    },
    skip: true,
  );

  testWidgets(
    'Midi mapping can be edited',
    (tester) async {
      final apiController = StreamController<ApiMessage>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockApiWebsocket();
      final apiSinkController = StreamController<ApiMessage>();
      final apiSend = _MockApiSend();
      apiSinkController.stream.listen(apiSend.call);
      when(api.stream).thenAnswer((_) => apiController.stream);
      when(api.sink).thenReturn(apiSinkController);
      when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
      when(api.isAlive).thenReturn(true);

      const getRequest =
          ApiMessage.midiMapping(message: MidiApiMessage.getRequest());

      when(apiSend(getRequest)).thenAnswer(
        (_) {
          apiController.add(
            const ApiMessage.midiMapping(
              message: MidiApiMessage.update(
                mapping: MidiMappingEntry(
                  id: 123,
                  mapping: MidiMapping.parameter(
                    midiChannel: 1,
                    ccNumber: 2,
                    parameterId: 'ampGain',
                  ),
                ),
              ),
            ),
          );
        },
      );

      await tester.pumpWidget(
        App(
          websocket: websocket,
          apiWebsocket: api,
        ),
      );

      verify(apiSend(getRequest));

      final midiMappingPage = await HomePageObject(tester).openMidiMapping();
      await tester.pumpAndSettle();

      expect(midiMappingPage.findPage(), findsOneWidget);
      expect(midiMappingPage.findMappingRows(), findsOneWidget);

      await midiMappingPage.updateMidiChannel(id: '123', value: 3);
      verify(
        apiSend(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.update(
              mapping: MidiMappingEntry(
                id: 123,
                mapping: MidiMapping.parameter(
                  midiChannel: 3,
                  ccNumber: 2,
                  parameterId: 'ampGain',
                ),
              ),
            ),
          ),
        ),
      );

      await midiMappingPage.updateCcNumber(id: '123', value: 4);
      verify(
        apiSend(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.update(
              mapping: MidiMappingEntry(
                id: 123,
                mapping: MidiMapping.parameter(
                  midiChannel: 3,
                  ccNumber: 4,
                  parameterId: 'ampGain',
                ),
              ),
            ),
          ),
        ),
      );

      await midiMappingPage.updateMode(
        id: '123',
        value: MidiMappingMode.toggle,
      );
      verify(
        apiSend(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.update(
              mapping: MidiMappingEntry(
                id: 123,
                mapping: MidiMapping.toggle(
                  midiChannel: 3,
                  ccNumber: 4,
                  parameterId: 'ampGain',
                ),
              ),
            ),
          ),
        ),
      );

      await midiMappingPage.updateParameter(
        id: '123',
        value: 'Valvestate: Contour',
      );
      verify(
        apiSend(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.update(
              mapping: MidiMappingEntry(
                id: 123,
                mapping: MidiMapping.toggle(
                  midiChannel: 3,
                  ccNumber: 4,
                  parameterId: 'contour',
                ),
              ),
            ),
          ),
        ),
      );

      // TODO Expect new mapping visible in UI

      await apiController.close();
    },
  );

  testWidgets(
    'MIDI mapping can be deleted',
    (tester) async {
      await tester.binding.setSurfaceSize(const Size(1920, 1080));

      final apiController = StreamController<ApiMessage>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockApiWebsocket();
      final apiSinkController = StreamController<ApiMessage>();
      final apiSend = _MockApiSend();
      apiSinkController.stream.listen(apiSend.call);
      when(api.stream).thenAnswer((_) => apiController.stream);
      when(api.sink).thenReturn(apiSinkController);
      when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
      when(api.isAlive).thenReturn(true);

      const getRequest =
          ApiMessage.midiMapping(message: MidiApiMessage.getRequest());

      when(apiSend(getRequest)).thenAnswer(
        (_) {
          apiController.add(
            const ApiMessage.midiMapping(
              message: MidiApiMessage.update(
                mapping: MidiMappingEntry(
                  id: 123,
                  mapping: MidiMapping.parameter(
                    midiChannel: 1,
                    ccNumber: 2,
                    parameterId: 'ampGain',
                  ),
                ),
              ),
            ),
          );
        },
      );

      await tester.pumpWidget(
        App(
          websocket: websocket,
          apiWebsocket: api,
        ),
      );

      verify(apiSend(getRequest));

      final midiMappingPage = await HomePageObject(tester).openMidiMapping();

      expect(midiMappingPage.findPage(), findsOneWidget);
      expect(midiMappingPage.findMappingRows(), findsOneWidget);

      await midiMappingPage.deleteMapping(id: '123');

      verify(
        apiSend(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.remove(
              id: 123,
            ),
          ),
        ),
      );
      expect(midiMappingPage.findMappingRows(), findsNothing);

      await apiController.close();
    },
  );

  testWidgets(
    'MIDI mapping can be learned',
    (tester) async {
      await tester.binding.setSurfaceSize(const Size(1920, 1080));

      final apiController = StreamController<ApiMessage>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockApiWebsocket();
      final apiSinkController = StreamController<ApiMessage>();
      final apiSend = _MockApiSend();
      apiSinkController.stream.listen(apiSend.call);
      when(api.stream).thenAnswer((_) => apiController.stream);
      when(api.sink).thenReturn(apiSinkController);
      when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
      when(api.isAlive).thenReturn(true);

      final sut = App(
        websocket: websocket,
        apiWebsocket: api,
      );

      final homePageObject = HomePageObject(tester);

      await tester.pumpWidget(sut);

      await homePageObject.toggleCollapsedAmplifier();

      await homePageObject.startMidiLearn();
      expect(
        homePageObject.findMidiLearnWaitingForParameterMessage(),
        findsOneWidget,
      );

      await homePageObject.dragKnob(parameterId: 'volume');
      expect(
        homePageObject.findMidiLearnWaitingForMidiMessage(),
        findsOneWidget,
      );

      const createRequest = ApiMessage.midiMapping(
        message: MidiApiMessage.createRequest(
          mapping: MidiMapping.parameter(
            midiChannel: 1,
            ccNumber: 2,
            parameterId: 'volume',
          ),
        ),
      );

      when(apiSend(createRequest)).thenAnswer(
        (_) {
          apiController.add(
            const ApiMessage.midiMapping(
              message: MidiApiMessage.createResponse(
                mapping: MidiMappingEntry(
                  id: 123,
                  mapping: MidiMapping.parameter(
                    midiChannel: 1,
                    ccNumber: 2,
                    parameterId: 'volume',
                  ),
                ),
              ),
            ),
          );
        },
      );

      // The MIDI message received event finishes the learning process, and
      // creates a the new mapping
      apiController.add(
        const ApiMessage.midiMapping(
          message: MidiApiMessage.midiMessageReceived(
            message: MidiMessage.controlChange(
              channel: 1,
              control: 2,
              value: 3,
            ),
          ),
        ),
      );

      await tester.pumpAndSettle();

      verify(apiSend(createRequest));
      expect(
        homePageObject.findMidiLearnWaitingForMidiMessage(),
        findsNothing,
      );

      final midiMappingPage = await homePageObject.openMidiMapping();
      expect(midiMappingPage.findPage(), findsOneWidget);
      // There should be one row, indicating the new mapping
      expect(midiMappingPage.findMappingRows(), findsOneWidget);

      await apiController.close();
    },
  );

  testWidgets(
    'MIDI mapping learn removes duplicates',
    (tester) async {
      await tester.binding.setSurfaceSize(const Size(1920, 1080));

      final apiController = StreamController<ApiMessage>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockApiWebsocket();
      final apiSinkController = StreamController<ApiMessage>();
      final apiSend = _MockApiSend();
      apiSinkController.stream.listen(apiSend.call);
      when(api.stream).thenAnswer((_) => apiController.stream);
      when(api.sink).thenReturn(apiSinkController);
      when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
      when(api.isAlive).thenReturn(true);

      const getRequest =
          ApiMessage.midiMapping(message: MidiApiMessage.getRequest());

      when(apiSend(getRequest)).thenAnswer(
        (_) {
          apiController.add(
            const ApiMessage.midiMapping(
              message: MidiApiMessage.update(
                mapping: MidiMappingEntry(
                  id: 456,
                  mapping: MidiMapping.parameter(
                    midiChannel: 3,
                    ccNumber: 4,
                    parameterId: 'volume',
                  ),
                ),
              ),
            ),
          );
        },
      );

      final homePageObject = HomePageObject(tester);

      await tester.pumpWidget(
        App(
          websocket: websocket,
          apiWebsocket: api,
        ),
      );

      await homePageObject.toggleCollapsedAmplifier();

      await homePageObject.startMidiLearn();
      expect(
        homePageObject.findMidiLearnWaitingForParameterMessage(),
        findsOneWidget,
      );

      await homePageObject.dragKnob(parameterId: 'volume');
      expect(
        homePageObject.findMidiLearnWaitingForMidiMessage(),
        findsOneWidget,
      );

      const createRequest = ApiMessage.midiMapping(
        message: MidiApiMessage.createRequest(
          mapping: MidiMapping.parameter(
            midiChannel: 1,
            ccNumber: 2,
            parameterId: 'volume',
          ),
        ),
      );

      when(apiSend(createRequest)).thenAnswer(
        (_) {
          apiController.add(
            const ApiMessage.midiMapping(
              message: MidiApiMessage.createResponse(
                mapping: MidiMappingEntry(
                  id: 123,
                  mapping: MidiMapping.parameter(
                    midiChannel: 1,
                    ccNumber: 2,
                    parameterId: 'volume',
                  ),
                ),
              ),
            ),
          );
        },
      );

      // The MIDI message received event finishes the learning process, and
      // creates a the new mapping
      apiController.add(
        const ApiMessage.midiMapping(
          message: MidiApiMessage.midiMessageReceived(
            message: MidiMessage.controlChange(
              channel: 1,
              control: 2,
              value: 3,
            ),
          ),
        ),
      );

      await tester.pumpAndSettle();

      verify(
        apiSend(
          const ApiMessage.midiMapping(
            message: MidiApiMessage.remove(id: 456),
          ),
        ),
      );

      verify(apiSend(createRequest));

      expect(
        homePageObject.findMidiLearnWaitingForMidiMessage(),
        findsNothing,
      );

      expect(
        homePageObject.findDuplicateMappingSnackbar(),
        findsOneWidget,
      );

      const createRequest2 = ApiMessage.midiMapping(
        message: MidiApiMessage.createRequest(
          mapping: MidiMapping.parameter(
            midiChannel: 3,
            ccNumber: 4,
            parameterId: 'volume',
          ),
        ),
      );

      when(apiSend(createRequest2)).thenAnswer(
        (_) {
          apiController.add(
            const ApiMessage.midiMapping(
              message: MidiApiMessage.createResponse(
                mapping: MidiMappingEntry(
                  id: 456,
                  mapping: MidiMapping.parameter(
                    midiChannel: 3,
                    ccNumber: 4,
                    parameterId: 'volume',
                  ),
                ),
              ),
            ),
          );
        },
      );

      await homePageObject.restoreRemovedDuplicateMappings();

      verify(apiSend(createRequest2));

      final midiMappingPage = await homePageObject.openMidiMapping();
      expect(midiMappingPage.findPage(), findsOneWidget);
      expect(midiMappingPage.findMappingRows(), findsNWidgets(2));

      await apiController.close();
    },
  );
}
