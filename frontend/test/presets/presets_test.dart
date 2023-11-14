import 'dart:async';
import 'dart:collection';

import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/audio_events.dart';
import 'package:shrapnel/json_websocket.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/util/uuid.dart';

import '../home_page_object.dart';
import 'presets_test.mocks.dart';

final _log = Logger('presets_test');

@GenerateMocks([UuidService])
@GenerateNiceMocks(
  [
    MockSpec<RobustWebsocket>(),
    MockSpec<JsonWebsocket>(),
    MockSpec<AudioClippingService>(),
    MockSpec<ParameterRepositoryBase>(),
  ],
)
void main() {
  setupLogger(Level.ALL);

  testWidgets('Presets test', (tester) async {
    await tester.runAsync(() async {
      final page = HomePageObject(tester);

      final parameterRepository = MockParameterRepositoryBase();
      final parameterController =
          StreamController<ParameterServiceInputMessage>();
      final outputMessages = Queue<ParameterServiceOutputMessage>();
      when(parameterRepository.sendMessage(any)).thenAnswer((realInvocation) {
        outputMessages.add(
          realInvocation.positionalArguments.single
              as ParameterServiceOutputMessage,
        );
      });
      when(parameterRepository.isAlive).thenReturn(true);
      when(parameterRepository.stream)
          .thenAnswer((_) => parameterController.stream);

      final websocket = MockRobustWebsocket();
      final jsonWebsocket = MockJsonWebsocket();
      when(jsonWebsocket.dataStream)
          .thenAnswer((_) => StreamController<Map<String, dynamic>>().stream);
      when(jsonWebsocket.connectionStream)
          .thenAnswer((_) => const Stream.empty());
      when(jsonWebsocket.isAlive).thenReturn(true);

      final uuid = MockUuidService();

      final sut = App(
        websocket: websocket,
        jsonWebsocket: jsonWebsocket,
        parameterRepository: parameterRepository,
        uuid: uuid,
      );

      _log.info('pumping');
      await tester.pumpWidget(sut);
      _log.info('pump done');
      _log.info('delaying');
      await Future<void>.delayed(const Duration(seconds: 5));
      _log.info('delay done');
      await tester.pumpAndSettle();

      expect(
        outputMessages.removeFirst(),
        ParameterServiceOutputMessage.requestInitialisation(),
      );
      expect(outputMessages, isEmpty);

      expect(page.presetsPage.getCurrentPresetName(), 'Default');
      expect(page.presetsPage.saveButton.isEnabled, isFalse);

      await page.dragKnob(parameterId: 'ampGain');

      expect(
        outputMessages.removeFirst(),
        isA<ParameterServiceOutputMessageParameterUpdate>()
            .having((message) => message.id, 'id', 'ampGain'),
      );
      expect(outputMessages, isEmpty);

      expect(page.presetsPage.saveButton.isEnabled, isTrue);
      expect(page.presetsPage.createButton.isEnabled, isTrue);
      page.presetsPage.saveButton.press();
      expect(page.presetsPage.saveButton.isEnabled, isFalse);
      expect(page.presetsPage.createButton.isEnabled, isFalse);

      // TODO check that the parameter change has been saved

      await page.dragKnob(parameterId: 'ampGain');
      expect(page.presetsPage.saveButton.isEnabled, isTrue);
      expect(page.presetsPage.createButton.isEnabled, isTrue);
      // TODO save new preset
      expect(page.presetsPage.saveButton.isEnabled, isFalse);
      expect(page.presetsPage.createButton.isEnabled, isFalse);

      // TODO
      // reload old preset
      // knob restored
      // reload new preset
      // knob restored again

      unawaited(parameterController.close());
    });
  });
}
