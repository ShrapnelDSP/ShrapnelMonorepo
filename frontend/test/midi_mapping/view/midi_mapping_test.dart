import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';
import 'package:shrapnel/json_websocket.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/midi_mapping/model/service.dart';
import 'package:shrapnel/midi_mapping/view/midi_mapping.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/util/uuid.dart';

import 'midi_mapping_test.mocks.dart';

class MidiMappingPageObject {
  const MidiMappingPageObject(this.tester);

  final WidgetTester tester;

  Finder findMappingRows() => find.byType(MidiChannelDropdown);
  Finder findPage() => find.text('MIDI Mapping');

  Future<MidiMappingCreatePageObject> openCreateDialog() async {
    await tester.tap(find.byType(FloatingActionButton));
    await tester.pumpAndSettle();
    return MidiMappingCreatePageObject(tester);
  }

  Future<void> updateMidiChannel({
    required String id,
    required int value,
  }) async {
    await tester.tap(find.byKey(Key('$id-midi-channel-dropdown')).last);
    await tester.pumpAndSettle();
    await tester.tap(find.text('$value').last);
    await tester.pumpAndSettle();
  }

  Future<void> updateCcNumber({
    required String id,
    required int value,
  }) async {
    await tester.tap(find.byKey(Key('$id-cc-number-dropdown')).last);
    await tester.pumpAndSettle();
    await tester.tap(find.text('$value').last);
    await tester.pumpAndSettle();
  }

  Future<void> updateParameter({
    required String id,
    required String value,
  }) async {
    await tester.tap(find.byKey(Key('$id-parameter-id-dropdown')).last);
    await tester.pumpAndSettle();
    await tester.tap(find.text(value).last);
    await tester.pumpAndSettle();
  }

  Future<void> deleteMapping({required String id}) async {
    await tester.tap(find.byKey(Key('$id-delete-button')).last);
    await tester.pumpAndSettle();
  }
}

class MidiMappingCreatePageObject {
  const MidiMappingCreatePageObject(this.tester);

  final WidgetTester tester;

  Future<void> selectMidiChannel(int channel) async {
    await tester.tap(
      find.ancestor(
        of: find.text('MIDI channel'),
        matching: find.byType(DropdownButton<int>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text('$channel').last);
    await tester.pumpAndSettle();
  }

  Future<void> selectCcNumber(int ccNumber) async {
    await tester.tap(
      find.ancestor(
        of: find.text('CC number'),
        matching: find.byType(DropdownButton<int>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text('$ccNumber').last);
    await tester.pumpAndSettle();
  }

  Future<void> selectParameter(String name) async {
    await tester.tap(
      find.ancestor(
        of: find.text('Parameter'),
        matching: find.byType(DropdownButton<String>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text(name).last);
    await tester.pumpAndSettle();
  }

  Future<void> submitCreateDialog() async {
    await tester.tap(find.text('Create'));
    await tester.pumpAndSettle();
  }
}

@GenerateMocks([JsonWebsocket, Uuid])
@GenerateNiceMocks([MockSpec<RobustWebsocket>()])
void main() {
  testWidgets('Midi mapping can be created', (tester) async {
    final apiController = StreamController<Map<String, dynamic>>.broadcast();
    final websocket = MockRobustWebsocket();
    final api = MockJsonWebsocket();
    final uuid = MockUuid();
    final sut = MultiProvider(
      providers: [
        ChangeNotifierProvider<RobustWebsocket>.value(value: websocket),
        ChangeNotifierProvider(
          create: (_) => ParameterService(websocket: websocket),
        ),
        Provider<JsonWebsocket>.value(value: api),
        ChangeNotifierProvider(
          create: (_) => MidiMappingService(websocket: api),
        ),
        ChangeNotifierProvider<Uuid>.value(value: uuid),
      ],
      child: const MyApp(),
    );

    await tester.pumpWidget(sut);

    final midiMappingPage = MidiMappingPageObject(tester);

    final getRequest = json.decodeAsMap(
      '''
      {
        "messageType": "MidiMap::get::request"
      }
      ''',
    );

    when(api.send(getRequest)).thenAnswer(
      (actualCall) {
        apiController.add(
          json.decodeAsMap(
            '''
            {
              "messageType": "MidiMap::get::response",
              "mappings": { }
            }
            ''',
          ),
        );
      },
    );

    when(api.stream).thenAnswer((_) => apiController.stream);

    // Open the page
    await tester.tap(find.byKey(const Key('midi-mapping-button')));
    await tester.pumpAndSettle();

    expect(midiMappingPage.findPage(), findsOneWidget);
    expect(midiMappingPage.findMappingRows(), findsNothing);
    verify(api.send(getRequest));

    when(uuid.v4()).thenReturn('123');

    final createRequest = json.decodeAsMap(
      '''
      {
        "messageType": "MidiMap::create::request",
        "mapping": {
          "123": {
            "midi_channel": 1,
            "cc_number": 2,
            "parameter_id": "noiseGateRelease"
          }
        }
      }
      ''',
    );

    when(api.send(createRequest)).thenAnswer(
      (_) {
        apiController.add(
          json.decodeAsMap(
            '''
            {
              "messageType": "MidiMap::create::response",
              "mapping": {
                "123": {
                  "midi_channel": 1,
                  "cc_number": 2,
                  "parameter_id": "noiseGateRelease"
                }
              }
            }
            ''',
          ),
        );
      },
    );

    final midiMappingCreatePage = await midiMappingPage.openCreateDialog();
    await midiMappingCreatePage.selectMidiChannel(1);
    await midiMappingCreatePage.selectCcNumber(2);
    // XXX: There is a bug in flutter where the DropdownButton's popup menu is
    //      unreliable during tests: https://github.com/flutter/flutter/issues/82908
    //
    // Pick an arbitrary parameter here. The only criteria for selection is that
    // it actually works during the test. This is more likely if something is
    // picked from the top of the list. When changing the parameter name, make
    // sure that the parameter ID in the stub is also updated.
    await midiMappingCreatePage.selectParameter('Noise Gate: Release');
    await midiMappingCreatePage.submitCreateDialog();

    verify(api.send(createRequest));

    // Expect new mapping visible in UI
    // TODO check the correct value is visible in all dropdowns
    expect(midiMappingPage.findMappingRows(), findsOneWidget);

    expect(apiController.hasListener, false);
    await apiController.close();
  });

  testWidgets('Midi mapping create rolls back on error', (tester) async {
    final apiController = StreamController<Map<String, dynamic>>.broadcast();
    final websocket = MockRobustWebsocket();
    final api = MockJsonWebsocket();
    final uuid = MockUuid();
    final sut = MultiProvider(
      providers: [
        ChangeNotifierProvider<RobustWebsocket>.value(value: websocket),
        ChangeNotifierProvider(
          create: (_) => ParameterService(websocket: websocket),
        ),
        Provider<JsonWebsocket>.value(value: api),
        ChangeNotifierProvider(
          create: (_) => MidiMappingService(websocket: api),
        ),
        ChangeNotifierProvider<Uuid>.value(value: uuid),
      ],
      child: const MyApp(),
    );

    await tester.pumpWidget(sut);

    final midiMappingPage = MidiMappingPageObject(tester);

    final getRequest = json.decodeAsMap(
      '''
      {
        "messageType": "MidiMap::get::request"
      }
      ''',
    );

    when(api.send(getRequest)).thenAnswer(
      (actualCall) {
        apiController.add(
          json.decodeAsMap(
            '''
            {
              "messageType": "MidiMap::get::response",
              "mappings": { }
            }
            ''',
          ),
        );
      },
    );

    when(api.stream).thenAnswer((_) => apiController.stream);

    // Open the page
    await tester.tap(find.byKey(const Key('midi-mapping-button')));
    await tester.pumpAndSettle();

    expect(midiMappingPage.findPage(), findsOneWidget);
    expect(midiMappingPage.findMappingRows(), findsNothing);

    when(uuid.v4()).thenReturn('123');

    // TODO the mapping ID is hardcoded
    // Dependency inject a UUID service, and make it return a fake value for
    // testing
    final createRequest = json.decodeAsMap(
      '''
      {
        "messageType": "MidiMap::create::request",
        "mapping": {
          "123": {
            "midi_channel": 1,
            "cc_number": 2,
            "parameter_id": "noiseGateRelease"
          }
        }
      }
      ''',
    );

    final midiMappingCreatePage = await midiMappingPage.openCreateDialog();
    await midiMappingCreatePage.selectMidiChannel(1);
    await midiMappingCreatePage.selectCcNumber(2);
    // XXX: There is a bug in flutter where the DropdownButton's popup menu is
    //      unreliable during tests: https://github.com/flutter/flutter/issues/82908
    //
    // Pick an arbitrary parameter here. The only criteria for selection is that
    // it actually works during the test. This is more likely if something is
    // picked from the top of the list. When changing the parameter name, make
    // sure that the parameter ID in the stub is also updated.
    await midiMappingCreatePage.selectParameter('Noise Gate: Release');
    await midiMappingCreatePage.submitCreateDialog();

    verify(api.send(createRequest));

    // Expect new mapping visible in UI
    expect(midiMappingPage.findMappingRows(), findsOneWidget);

    await tester.pump(const Duration(seconds: 1));

    expect(
      midiMappingPage.findMappingRows(),
      findsNothing,
      reason: 'UI was not rolled back after create request timeout',
    );

    expect(apiController.hasListener, false);
    await apiController.close();
  });

  testWidgets(
    'Midi mapping can be edited',
    (tester) async {
      final apiController = StreamController<Map<String, dynamic>>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockJsonWebsocket();
      final sut = MultiProvider(
        providers: [
          ChangeNotifierProvider<RobustWebsocket>.value(value: websocket),
          ChangeNotifierProvider(
            create: (_) => ParameterService(websocket: websocket),
          ),
          Provider<JsonWebsocket>.value(value: api),
          ChangeNotifierProvider(
            create: (_) => MidiMappingService(websocket: api),
          ),
        ],
        child: const MyApp(),
      );

      await tester.pumpWidget(sut);

      final midiMappingPage = MidiMappingPageObject(tester);

      final getRequest = json.decodeAsMap(
        '''
        {
          "messageType": "MidiMap::get::request"
        }
        ''',
      );

      when(api.send(getRequest)).thenAnswer(
        (actualCall) {
          apiController.add(
            json.decodeAsMap(
              '''
              {
                "messageType": "MidiMap::get::response",
                "mappings": {
                  "123": { "midi_channel": 1, "cc_number": 2, "parameter_id": "ampGain" }
                }
              }
              ''',
            ),
          );
        },
      );

      when(api.stream).thenAnswer((_) => apiController.stream);

      // Open the page
      await tester.tap(find.byKey(const Key('midi-mapping-button')));
      await tester.pumpAndSettle();

      expect(midiMappingPage.findPage(), findsOneWidget);
      expect(midiMappingPage.findMappingRows(), findsOneWidget);
      verify(api.send(getRequest));

      await midiMappingPage.updateMidiChannel(id: '123', value: 3);
      verify(
        api.send(
          json.decodeAsMap(
            '''
            {
              "messageType": "MidiMap::update",
              "mapping": {
                "123": {
                  "midi_channel": 3,
                  "cc_number": 2,
                  "parameter_id": "ampGain"
                }
              }
            }
            ''',
          ),
        ),
      );

      await midiMappingPage.updateCcNumber(id: '123', value: 4);
      verify(
        api.send(
          json.decodeAsMap(
            '''
            {
              "messageType": "MidiMap::update",
              "mapping": {
                "123": {
                  "midi_channel": 3,
                  "cc_number": 4,
                  "parameter_id": "ampGain"
                }
              }
            }
            ''',
          ),
        ),
      );

      await midiMappingPage.updateParameter(
        id: '123',
        value: 'Valvestate: Contour',
      );
      verify(
        api.send(
          json.decodeAsMap(
            '''
            {
              "messageType": "MidiMap::update",
              "mapping": {
                "123": {
                  "midi_channel": 3,
                  "cc_number": 4,
                  "parameter_id": "contour"
                }
              }
            }
            ''',
          ),
        ),
      );

      // TODO Expect new mapping visible in UI

      expect(apiController.hasListener, false);
      await apiController.close();
    },
  );

  testWidgets(
    'MIDI mapping can be deleted',
    (tester) async {
      await tester.binding.setSurfaceSize(const Size(1920, 1080));

      final apiController = StreamController<Map<String, dynamic>>.broadcast();
      final websocket = MockRobustWebsocket();
      final api = MockJsonWebsocket();
      final sut = MultiProvider(
        providers: [
          ChangeNotifierProvider<RobustWebsocket>.value(value: websocket),
          ChangeNotifierProvider(
            create: (_) => ParameterService(websocket: websocket),
          ),
          Provider<JsonWebsocket>.value(value: api),
          ChangeNotifierProvider(
            create: (_) => MidiMappingService(websocket: api),
          ),
        ],
        child: const MyApp(),
      );

      await tester.pumpWidget(sut);

      final midiMappingPage = MidiMappingPageObject(tester);

      final getRequest = json.decodeAsMap(
        '''
        {
          "messageType": "MidiMap::get::request"
        }
        ''',
      );

      when(api.send(getRequest)).thenAnswer(
        (actualCall) {
          apiController.add(
            json.decodeAsMap(
              '''
              {
                "messageType": "MidiMap::get::response",
                "mappings": {
                  "123": { "midi_channel": 1, "cc_number": 2, "parameter_id": "ampGain" }
                }
              }
              ''',
            ),
          );
        },
      );

      when(api.stream).thenAnswer((_) => apiController.stream);

      // Open the page
      await tester.tap(find.byKey(const Key('midi-mapping-button')));
      await tester.pumpAndSettle();

      expect(midiMappingPage.findPage(), findsOneWidget);
      expect(midiMappingPage.findMappingRows(), findsOneWidget);
      verify(api.send(getRequest));

      await midiMappingPage.deleteMapping(id: '123');

      verify(
        api.send(
          json.decodeAsMap(
            '''
          {
            "messageType": "MidiMap::remove",
            "id": "123"
          }
          ''',
          ),
        ),
      );
      expect(midiMappingPage.findMappingRows(), findsNothing);

      expect(apiController.hasListener, false);
      await apiController.close();
    },
  );
}

extension on JsonCodec {
  Map<String, dynamic> decodeAsMap(String s) =>
      json.decode(s) as Map<String, dynamic>;
}
