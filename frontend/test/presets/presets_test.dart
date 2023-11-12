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
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/audio_events.dart';
import 'package:shrapnel/json_websocket.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/util/uuid.dart';

import '../home_page_object.dart';
import 'presets_test.mocks.dart';

@GenerateMocks([JsonWebsocket, Uuid])
@GenerateNiceMocks(
  [MockSpec<RobustWebsocket>(), MockSpec<AudioClippingService>()],
)
void main() {
  testWidgets('Presets test', (tester) async {
    final page = HomePageObject(tester);

    final websocket = MockRobustWebsocket();
    final api = MockJsonWebsocket();
    when(api.dataStream).thenAnswer((_) => apiController.stream);
    when(api.connectionStream).thenAnswer((_) => Stream.fromIterable([]));
    when(api.isAlive).thenReturn(true);
    final uuid = MockUuid();

    final sut = App(
      websocket: websocket,
      jsonWebsocket: api,
      uuid: uuid,
    );

    await tester.pumpWidget(sut);

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
}
