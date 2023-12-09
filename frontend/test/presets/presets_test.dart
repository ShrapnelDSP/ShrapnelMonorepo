import 'dart:async';
import 'dart:collection';
import 'dart:ui';

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

import '../home_page_object.dart';
import 'presets_test.mocks.dart';

final _log = Logger('presets_test');

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
    tester.view.physicalSize = const Size(4000, 3000);
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

      void updateParameter(String id, double value) {
        parameterValues[id] = value;
        parameterController.add(
          ParameterServiceInputMessageParameterUpdate(
            parameter: AudioParameterDoubleData(id: id, value: value),
          ),
        );
      }

      void loadPreset(PresetState preset) {
        updateParameter('ampGain', preset.parameters.ampGain);
        updateParameter('ampChannel', preset.parameters.ampChannel);
        updateParameter('bass', preset.parameters.bass);
        updateParameter('middle', preset.parameters.middle);
        updateParameter('treble', preset.parameters.treble);
        updateParameter('contour', preset.parameters.contour);
        updateParameter('volume', preset.parameters.volume);
        updateParameter(
          'noiseGateThreshold',
          preset.parameters.noiseGateThreshold,
        );
        updateParameter(
          'noiseGateHysteresis',
          preset.parameters.noiseGateHysteresis,
        );
        updateParameter('noiseGateAttack', preset.parameters.noiseGateAttack);
        updateParameter('noiseGateHold', preset.parameters.noiseGateHold);
        updateParameter('noiseGateRelease', preset.parameters.noiseGateRelease);
        updateParameter('noiseGateBypass', preset.parameters.noiseGateBypass);
        updateParameter('chorusRate', preset.parameters.chorusRate);
        updateParameter('chorusDepth', preset.parameters.chorusDepth);
        updateParameter('chorusMix', preset.parameters.chorusMix);
        updateParameter('chorusBypass', preset.parameters.chorusBypass);
        updateParameter('wahPosition', preset.parameters.wahPosition);
        updateParameter('wahVocal', preset.parameters.wahVocal);
        updateParameter('wahBypass', preset.parameters.wahBypass);
      }

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
            updateParameter(id, value);
        }
      });
      when(parameterRepository.isAlive).thenReturn(true);
      when(parameterRepository.stream)
          .thenAnswer((_) => parameterController.stream);

      const initialPresetId = 0;

      final presetsRepository = MockPresetsRepositoryBase();
      const initialPresetName = 'Test Preset';
      final presetsSubject = BehaviorSubject.seeded(<int, PresetRecord>{
        initialPresetId: PresetRecord(
          id: initialPresetId,
          preset: PresetState(
            name: initialPresetName,
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
        ),
      });
      when(presetsRepository.presets).thenAnswer((_) => presetsSubject);
      when(presetsRepository.create(any)).thenAnswer((realInvocation) async {
        final preset = realInvocation.positionalArguments.single as PresetState;
        // FIXME: implement fake incrementing id
        presetsSubject.add({...presetsSubject.value, preset.id: preset});
      });
      when(presetsRepository.update(any)).thenAnswer((realInvocation) async {
        final preset = realInvocation.positionalArguments.single as PresetRecord;
        presetsSubject.add({...presetsSubject.value, preset.id: preset});
      });
      when(presetsRepository.delete(any)).thenAnswer((realInvocation) async {
        final id = realInvocation.positionalArguments.single as int;
        presetsSubject.add({...presetsSubject.value}..remove(id));
      });

      final selectedPresetRepository = MockSelectedPresetRepositoryBase();
      final selectedPresetSubject = BehaviorSubject.seeded(initialPresetId);
      when(selectedPresetRepository.selectedPreset)
          .thenAnswer((_) => selectedPresetSubject);
      when(selectedPresetRepository.selectPreset(any))
          .thenAnswer((realInvocation) async {
        final id = realInvocation.positionalArguments.single as int;
        selectedPresetSubject.add(id);

        loadPreset(presetsSubject.value[id]!.preset);
      });

      final websocket = MockRobustWebsocket();
      final jsonWebsocket = MockJsonWebsocket();
      when(jsonWebsocket.dataStream)
          .thenAnswer((_) => StreamController<Map<String, dynamic>>().stream);
      when(jsonWebsocket.connectionStream)
          .thenAnswer((_) => const Stream.empty());
      when(jsonWebsocket.isAlive).thenReturn(true);

      final sut = App(
        websocket: websocket,
        jsonWebsocket: jsonWebsocket,
        parameterRepository: parameterRepository,
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

      expect(page.presetsPage.getCurrentPresetName(), initialPresetName);
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
      await tester.pumpAndSettle();

      verify(presetsRepository.update(any)).called(1);

      expect(page.presetsPage.saveButton.isEnabled, isFalse);
      expect(page.presetsPage.createButton.isEnabled, isTrue);

      await page.dragKnob(parameterId: 'ampGain');
      await Future<void>.delayed(const Duration(milliseconds: 200));

      expect(page.presetsPage.saveButton.isEnabled, isTrue);
      expect(page.presetsPage.createButton.isEnabled, isTrue);

      const newPresetName = 'New Preset';
      await page.presetsPage.createPreset(newPresetName);

      verify(presetsRepository.create(any)).called(1);

      expect(page.presetsPage.saveButton.isEnabled, isFalse);
      expect(page.presetsPage.createButton.isEnabled, isTrue);

      await page.presetsPage.selectPreset(initialPresetName);
      expect(
        page.getKnobValue(parameterId: 'ampGain'),
        within(
          distance: 0.01,
          from: presetsSubject.value[initialPresetId]!.preset.parameters.ampGain,
        ),
      );

      await page.presetsPage.selectPreset(newPresetName);
      expect(
        page.getKnobValue(parameterId: 'ampGain'),
        within(
          distance: 0.01,
          from: presetsSubject.value[newPresetId]!.preset.parameters.ampGain,
        ),
      );

      unawaited(parameterController.close());
      unawaited(presetsSubject.close());
      unawaited(selectedPresetSubject.close());
    });
  });
}
