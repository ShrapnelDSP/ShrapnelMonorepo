import 'dart:async';
import 'dart:collection';

import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:rxdart/rxdart.dart';
import 'package:shrapnel/audio_events.dart';
import 'package:shrapnel/json_websocket.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/presets/model/presets.dart';
import 'package:shrapnel/presets/model/presets_service.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/util/uuid.dart';
import 'package:uuid/uuid.dart';

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
    MockSpec<PresetsRepositoryBase>(),
    MockSpec<SelectedPresetRepositoryBase>(),
  ],
)
void main() {
  setupLogger(Level.ALL);

  testWidgets('Presets test', (tester) async {
    await tester.runAsync(() async {
      final page = HomePageObject(tester);

      final parameterValues = <String, double>{
        'ampGain': 0.1,
        'ampChannel': 1,
        'bass': 0.2,
        'middle': 0.3,
        'treble': 0.4,
        'contour': 0.6,
        'volume': 0.7,
        'noiseGateThreshold': 0.8,
        'noiseGateHysteresis': 0.9,
        'noiseGateAttack': 1.0,
        'noiseGateHold': 0.1,
        'noiseGateRelease': 0.2,
        'noiseGateBypass': 0.3,
        'chorusRate': 0.4,
        'chorusDepth': 0.6,
        'chorusMix': 0.7,
        'chorusBypass': 0.8,
        'wahPosition': 0.9,
        'wahVocal': 1.0,
        'wahBypass': 0.1,
      };
      final parameterRepository = MockParameterRepositoryBase();
      final parameterController =
          StreamController<ParameterServiceInputMessage>();
      final parameterMessages = Queue<ParameterServiceOutputMessage>();
      when(parameterRepository.sendMessage(any)).thenAnswer((realInvocation) {
        final message = realInvocation.positionalArguments.single
            as ParameterServiceOutputMessage;
        parameterMessages.add(message);

        switch (message) {
          case ParameterServiceOutputMessageRequestInitialisation():
            for (final entry in parameterValues.entries) {
              parameterController.add(
                ParameterServiceInputMessageParameterUpdate(
                  parameter: AudioParameterDoubleData(
                    id: entry.key,
                    value: entry.value,
                  ),
                ),
              );
            }
          case ParameterServiceOutputMessageParameterUpdate(
              :final id,
              :final value
            ):
            parameterValues[id] = value;
        }
      });
      when(parameterRepository.isAlive).thenReturn(true);
      when(parameterRepository.stream)
          .thenAnswer((_) => parameterController.stream);

      final presetId = UuidValue('00000000-0000-0000-0000-000000000000');

      final presetsRepository = MockPresetsRepositoryBase();
      final presetsSubject = BehaviorSubject.seeded(<UuidValue, PresetState>{
        presetId: PresetState(
          id: presetId,
          name: 'Test Preset',
          parameters: PresetParametersData(
            ampGain: 0.1,
            ampChannel: 1,
            bass: 0.2,
            middle: 0.3,
            treble: 0.4,
            contour: 0.6,
            volume: 0.7,
            noiseGateThreshold: 0.8,
            noiseGateHysteresis: 0.9,
            noiseGateAttack: 1.0,
            noiseGateHold: 0.1,
            noiseGateRelease: 0.2,
            noiseGateBypass: 0.3,
            chorusRate: 0.4,
            chorusDepth: 0.6,
            chorusMix: 0.7,
            chorusBypass: 0.8,
            wahPosition: 0.9,
            wahVocal: 1.0,
            wahBypass: 0.1,
          ),
        ),
      });
      when(presetsRepository.presets).thenAnswer((_) => presetsSubject);

      final selectedPresetRepository = MockSelectedPresetRepositoryBase();
      final selectedPresetSubject = BehaviorSubject.seeded(presetId);
      when(selectedPresetRepository.selectedPreset)
          .thenAnswer((_) => selectedPresetSubject);

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
        presetsRepository: presetsRepository,
        selectedPresetRepository: selectedPresetRepository,
      );

      await tester.pumpWidget(sut);
      _log.info('pump done');
      _log.info('delaying');
      // For debouncing of the parameter output messages
      await Future<void>.delayed(const Duration(milliseconds: 200));
      _log.info('delay done');
      await tester.pumpAndSettle();
      await pumpEventQueue();
      await Future<void>.delayed(Duration.zero);
      await pumpEventQueue();

      expect(
        parameterMessages.removeFirst(),
        ParameterServiceOutputMessage.requestInitialisation(),
      );
      expect(parameterMessages, isEmpty);

      expect(page.presetsPage.getCurrentPresetName(), 'Test Preset');
      expect(page.presetsPage.saveButton.isEnabled, isFalse);

      await page.toggleCollapsedAmplifier();
      await page.dragKnob(parameterId: 'ampGain');
      await Future<void>.delayed(const Duration(milliseconds: 200));

      expect(
        parameterMessages.removeFirst(),
        isA<ParameterServiceOutputMessageParameterUpdate>()
            .having((message) => message.id, 'id', 'ampGain'),
      );
      expect(parameterMessages, isEmpty);

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
      unawaited(presetsSubject.close());
      unawaited(selectedPresetSubject.close());
    });
  });
}
