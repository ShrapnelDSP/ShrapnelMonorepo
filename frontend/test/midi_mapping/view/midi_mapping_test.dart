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
    final valvestateGainParameter = find.text(name).last;
    await tester.tap(valvestateGainParameter);
    await tester.pumpAndSettle();
  }

  Future<void> submitCreateDialog() async {
    await tester.tap(find.text('Create'));
    await tester.pumpAndSettle();
  }
}

@GenerateMocks([JsonWebsocket])
@GenerateNiceMocks([MockSpec<RobustWebsocket>()])
void main() {
  testWidgets('Midi mapping can be created', (tester) async {
    final apiController = StreamController<Map<String, dynamic>>.broadcast();
    final websocket = MockRobustWebsocket();

    final api = MockJsonWebsocket();
    throwOnMissingStub(api);

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
    expect(midiMappingPage.findMappingRows(), findsOneWidget);

    await apiController.close();
  });

  testWidgets(
    'Midi mapping can be edited',
    (tester) async {
      //await tester.pumpWidget();

      // Open the page

      // Expect get request, and respond with a mapping

      // Change a field of the mapping

      // Expect JSON message to update mapping transmitted

      // Expect new mapping visible in UI
    },
    skip: true,
  );
}

extension on JsonCodec {
  Map<String, dynamic> decodeAsMap(String s) =>
      json.decode(s) as Map<String, dynamic>;
}
