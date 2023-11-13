// Launch
// Default preset visible
// Save button disabled
// move knob
// Save button enabled
// Save
// Save grayed out
// Move knob
// save new preset
// save grayed out
// reload old preset
// knob restored
// reload new preset
// knot restored again

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

@GenerateMocks([Uuid])
@GenerateNiceMocks(
  [
    MockSpec<RobustWebsocket>(),
    MockSpec<JsonWebsocket>(),
    MockSpec<AudioClippingService>(),
    MockSpec<ParameterRepositoryBase>()
  ],
)
void main() {
  setupLogger(Level.ALL);

  testWidgets('Presets test', (tester) async {
    await tester.runAsync(() async {
      final page = HomePageObject(tester);

      final parameterRepository = MockParameterRepositoryBase();
      when(parameterRepository.isAlive).thenReturn(true);

      final websocket = MockRobustWebsocket();
      final jsonWebsocket = MockJsonWebsocket();
      when(jsonWebsocket.dataStream).thenAnswer((_) => const Stream.empty());
      when(jsonWebsocket.connectionStream)
          .thenAnswer((_) => const Stream.empty());
      when(jsonWebsocket.isAlive).thenReturn(true);

      final uuid = MockUuid();

      final sut = App(
        websocket: websocket,
        jsonWebsocket: jsonWebsocket,
        parameterRepository: parameterRepository,
        uuid: uuid,
      );

      await tester.pumpWidget(sut);
      await tester.pumpAndSettle();
      await tester.pumpAndSettle();
      await tester.pumpAndSettle();
      await tester.pumpAndSettle();
      await tester.pumpAndSettle();

      expect(page.presetsPage.getCurrentPresetName(), 'Default');
      expect(page.presetsPage.saveButton.isEnabled, isFalse);

      await page.dragKnob(parameterId: 'ampGain');

      expect(page.presetsPage.saveButton.isEnabled, isTrue);
      page.presetsPage.saveButton.press();
      expect(page.presetsPage.saveButton.isEnabled, isFalse);

      // TODO check that the parameter change has been saved

      await page.dragKnob(parameterId: 'ampGain');
      expect(page.presetsPage.createButton.isEnabled, isTrue);
      // TODO finish saving the preset
      expect(page.presetsPage.createButton.isEnabled, isFalse);

      // TODO reload etc.
    });
  });
}
