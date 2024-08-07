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

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:flutter/material.dart';
import 'package:flutter_state_notifier/flutter_state_notifier.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:logging/logging.dart';
import 'package:provider/provider.dart';
import 'package:rxdart/rxdart.dart';

import 'api/api_websocket.dart';
import 'audio_events.dart';
import 'chorus.dart';
import 'core/message_transport.dart';
import 'heavy_metal.dart';
import 'midi_mapping/model/midi_learn.dart';
import 'midi_mapping/model/midi_learn_state.dart';
import 'midi_mapping/model/service.dart';
import 'midi_mapping/view/midi_learn.dart';
import 'midi_mapping/view/midi_mapping.dart';
import 'noise_gate.dart';
import 'parameter.dart';
import 'pedalboard.dart';
import 'presets/model/presets.dart';
import 'presets/model/presets_client.dart' as presets_client;
import 'presets/model/presets_repository.dart';
import 'presets/model/presets_service.dart';
import 'presets/model/selected_preset_client.dart';
import 'presets/model/selected_preset_repository.dart';
import 'presets/view/presets.dart';
import 'robust_websocket.dart';
import 'status/data/status.dart';
import 'status/model/websocket_status.dart';
import 'status/view/websocket_status.dart';
import 'tube_screamer.dart';
import 'valvestate.dart';
import 'wah.dart';
import 'wifi_provisioning.dart';

final _log = Logger('shrapnel.main');

String formatDateTime(DateTime t) {
  final builder = StringBuffer()
    ..write(t.hour.toString().padLeft(2, '0'))
    ..write(':')
    ..write(
      t.minute.toString().padLeft(2, '0'),
    )
    ..write(':')
    ..write(
      t.second.toString().padLeft(2, '0'),
    )
    ..write('.')
    ..write(
      t.millisecond.toString().padLeft(3, '0'),
    );

  return builder.toString();
}

void main() {
  setupLogger(Level.INFO);
  GoogleFonts.config.allowRuntimeFetching = false;
  runApp(App());
}

void setupLogger(Level level) {
  hierarchicalLoggingEnabled = true;
  Logger.root.level = level;
  Logger.root.onRecord.listen((record) {
    debugPrint(
      '${record.level.name.padLeft("WARNING".length)} '
      '${formatDateTime(record.time)} '
      '${record.loggerName}: '
      '${record.message}',
    );
  });
}

class App extends StatelessWidget {
  App({
    super.key,
    RobustWebsocket? websocket,
    ApiWebsocket? apiWebsocket,
    WifiProvisioningService? provisioning,
    ReconnectingMessageTransport<ParameterServiceOutputMessage,
            ParameterServiceInputMessage>?
        parameterTransport,
    PresetsRepositoryBase? presetsRepository,
    ParameterService? parameterService,
    SelectedPresetRepositoryBase? selectedPresetRepository,
    String? normalHost,
    String? provisioningHost,
  }) {
    provisioningHost ??= 'guitar-dsp.local';
    normalHost ??= 'guitar-dsp.local';

    websocket ??= RobustWebsocket(
      uri: Uri.parse('http://guitar-dsp.local:8080/websocket')
          .replace(host: normalHost),
    );
    _websocket = websocket;
    apiWebsocket ??= ApiWebsocket(websocket: websocket);
    audioClippingService = AudioClippingService(
      stream: apiWebsocket.stream
          .whereType<ApiMessageAudioEvent>()
          .map((event) => event.message),
    );
    this.provisioning = provisioning ??
        WifiProvisioningService(
          provisioningFactory: () {
            _log.info('Creating provisioning connection');
            return Provisioning(
              security: Security1(pop: 'abcd1234'),
              transport: TransportHTTP(provisioningHost!),
            );
          },
        );

    midiMappingService = MidiMappingService(
      websocket: MidiMappingTransport(websocket: apiWebsocket),
    );
    this.parameterService = parameterService ??
        ParameterService(
          transport:
              parameterTransport ?? ParameterTransport(websocket: apiWebsocket),
        );
    midiLearnService = MidiLearnService(
      mappingService: midiMappingService,
    );

    this
        .parameterService
        .parameterUpdates
        .map((e) => e.id)
        .listen(midiLearnService.parameterUpdated);

    apiWebsocket.stream
        .whereType<ApiMessageMidiMapping>()
        .map((event) => event.message)
        .listen(
          (m) => m.maybeWhen(
            midiMessageReceived: midiLearnService.midiMessageReceived,
            orElse: () => null,
          ),
        );

    this.presetsRepository = presetsRepository ??
        PresetsRepository(
          client: presets_client.PresetsClient(
            transport: presets_client.PresetsTransport(websocket: apiWebsocket),
          ),
        );
    this.selectedPresetRepository = selectedPresetRepository ??
        SelectedPresetRepository(
          client: SelectedPresetClient(
            transport: SelectedPresetTransport(websocket: apiWebsocket),
          ),
        );
  }

  late final RobustWebsocket _websocket;
  late final AudioClippingService audioClippingService;
  late final WifiProvisioningService provisioning;
  late final MidiLearnService midiLearnService;
  late final ParameterService parameterService;
  late final MidiMappingService midiMappingService;
  late final PresetsRepositoryBase presetsRepository;
  late final SelectedPresetRepositoryBase selectedPresetRepository;

  @override
  Widget build(BuildContext context) {
    return MultiProvider(
      providers: [
        StateNotifierProvider<MidiLearnService, MidiLearnState>.value(
          value: midiLearnService,
        ),
        StateNotifierProvider<WebSocketStatusModel, WebSocketStatusData>(
          create: (_) => WebSocketStatusModel(websocket: _websocket),
        ),
        ChangeNotifierProvider.value(value: provisioning),
        ChangeNotifierProvider.value(
          value: parameterService,
        ),
        ChangeNotifierProvider.value(
          value: midiMappingService,
        ),
        ChangeNotifierProvider.value(value: audioClippingService),
        Provider(
          create: (_) => ChorusModel(parameterService: parameterService),
          lazy: false,
        ),
        Provider(
          create: (_) => HeavyMetalModel(parameterService: parameterService),
          lazy: false,
        ),
        Provider(
          create: (_) => NoiseGateModel(parameterService: parameterService),
          lazy: false,
        ),
        Provider(
          create: (_) => TubeScreamerModel(parameterService: parameterService),
          lazy: false,
        ),
        Provider(
          create: (_) => ValvestateModel(parameterService: parameterService),
          lazy: false,
        ),
        Provider(
          create: (_) => WahModel(parameterService: parameterService),
          lazy: false,
        ),
        Provider.value(value: presetsRepository),
        Provider.value(value: selectedPresetRepository),
        StateNotifierProvider<PresetsServiceBase, PresetsState>(
          create: (_) {
            return PresetsService(
              presetsRepository: presetsRepository,
              selectedPresetRepository: selectedPresetRepository,
              parametersState: ParametersMergeStream(
                ampGain: parameterService.getParameter('ampGain').value,
                ampChannel: parameterService.getParameter('ampChannel').value,
                bass: parameterService.getParameter('bass').value,
                middle: parameterService.getParameter('middle').value,
                treble: parameterService.getParameter('treble').value,
                contour: parameterService.getParameter('contour').value,
                volume: parameterService.getParameter('volume').value,
                noiseGateThreshold:
                    parameterService.getParameter('noiseGateThreshold').value,
                noiseGateHysteresis:
                    parameterService.getParameter('noiseGateHysteresis').value,
                noiseGateAttack:
                    parameterService.getParameter('noiseGateAttack').value,
                noiseGateHold:
                    parameterService.getParameter('noiseGateHold').value,
                noiseGateRelease:
                    parameterService.getParameter('noiseGateRelease').value,
                noiseGateBypass:
                    parameterService.getParameter('noiseGateBypass').value,
                chorusRate: parameterService.getParameter('chorusRate').value,
                chorusDepth: parameterService.getParameter('chorusDepth').value,
                chorusMix: parameterService.getParameter('chorusMix').value,
                chorusBypass:
                    parameterService.getParameter('chorusBypass').value,
                wahPosition: parameterService.getParameter('wahPosition').value,
                wahVocal: parameterService.getParameter('wahVocal').value,
                wahBypass: parameterService.getParameter('wahBypass').value,
              ).stream,
            );
          },
        ),
      ],
      child: const MyApp(),
    );
  }
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ShrapnelDSP',
      theme: ThemeData.from(
        colorScheme: ColorScheme(
          primary: Colors.orange,
          secondary: Colors.red,
          surface: Colors.grey[800]!,
          background: Colors.grey[850]!,
          error: Colors.red[700]!,
          onPrimary: Colors.black,
          onSecondary: Colors.white,
          onSurface: Colors.white,
          onBackground: Colors.white,
          onError: Colors.black,
          brightness: Brightness.dark,
        ),
        textTheme: Typography().white.apply(
              fontFamily: 'Noto Sans',
            ),
        useMaterial3: false,
      ),
      home: const MyHomePage(title: 'ShrapnelDSP'),
      debugShowCheckedModeBanner: false,
    );
  }
}

class ParametersMergeStream {
  ParametersMergeStream({
    required this.ampGain,
    required this.ampChannel,
    required this.bass,
    required this.middle,
    required this.treble,
    required this.contour,
    required this.volume,
    required this.noiseGateThreshold,
    required this.noiseGateHysteresis,
    required this.noiseGateAttack,
    required this.noiseGateHold,
    required this.noiseGateRelease,
    required this.noiseGateBypass,
    required this.chorusRate,
    required this.chorusDepth,
    required this.chorusMix,
    required this.chorusBypass,
    required this.wahPosition,
    required this.wahVocal,
    required this.wahBypass,
  }) {
    _controller =
        BehaviorSubject<PresetParametersData>.seeded(getParametersState());

    void updateState() {
      _controller.add(getParametersState());
    }

    ampGain.listen((_) => updateState());
    ampChannel.listen((_) => updateState());
    bass.listen((_) => updateState());
    middle.listen((_) => updateState());
    treble.listen((_) => updateState());
    contour.listen((_) => updateState());
    volume.listen((_) => updateState());
    noiseGateThreshold.listen((_) => updateState());
    noiseGateHysteresis.listen((_) => updateState());
    noiseGateAttack.listen((_) => updateState());
    noiseGateHold.listen((_) => updateState());
    noiseGateRelease.listen((_) => updateState());
    noiseGateBypass.listen((_) => updateState());
    chorusRate.listen((_) => updateState());
    chorusDepth.listen((_) => updateState());
    chorusMix.listen((_) => updateState());
    chorusBypass.listen((_) => updateState());
    wahPosition.listen((_) => updateState());
    wahVocal.listen((_) => updateState());
    wahBypass.listen((_) => updateState());
  }

  late final BehaviorSubject<PresetParametersData> _controller;

  final ValueStream<double> ampGain;
  final ValueStream<double> ampChannel;
  final ValueStream<double> bass;
  final ValueStream<double> middle;
  final ValueStream<double> treble;
  final ValueStream<double> contour;
  final ValueStream<double> volume;
  final ValueStream<double> noiseGateThreshold;
  final ValueStream<double> noiseGateHysteresis;
  final ValueStream<double> noiseGateAttack;
  final ValueStream<double> noiseGateHold;
  final ValueStream<double> noiseGateRelease;
  final ValueStream<double> noiseGateBypass;
  final ValueStream<double> chorusRate;
  final ValueStream<double> chorusDepth;
  final ValueStream<double> chorusMix;
  final ValueStream<double> chorusBypass;
  final ValueStream<double> wahPosition;
  final ValueStream<double> wahVocal;
  final ValueStream<double> wahBypass;

  ValueStream<PresetParametersData> get stream => _controller.stream;

  PresetParametersData getParametersState() {
    final parameters = PresetParametersData(
      ampGain: ampGain.value,
      ampChannel: ampChannel.value,
      bass: bass.value,
      middle: middle.value,
      treble: treble.value,
      contour: contour.value,
      volume: volume.value,
      noiseGateThreshold: noiseGateThreshold.value,
      noiseGateHysteresis: noiseGateHysteresis.value,
      noiseGateAttack: noiseGateAttack.value,
      noiseGateHold: noiseGateHold.value,
      noiseGateRelease: noiseGateRelease.value,
      noiseGateBypass: noiseGateBypass.value,
      chorusRate: chorusRate.value,
      chorusDepth: chorusDepth.value,
      chorusMix: chorusMix.value,
      chorusBypass: chorusBypass.value,
      wahPosition: wahPosition.value,
      wahVocal: wahVocal.value,
      wahBypass: wahBypass.value,
    );

    return parameters;
  }
}

class MyHomePage extends StatelessWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
        actions: [
          Tooltip(
            message: 'MIDI Learn',
            child: IconButton(
              icon: const Icon(Icons.menu_book_outlined),
              key: const Key('midi-learn-button'),
              onPressed: () {
                context.read<MidiLearnService>().startLearning();
              },
            ),
          ),
          Tooltip(
            message: 'MIDI Mapping',
            child: IconButton(
              icon: const Icon(Icons.map_outlined),
              key: const Key('midi-mapping-button'),
              onPressed: () {
                unawaited(
                  Navigator.push(
                    context,
                    MaterialPageRoute<MidiMappingPage>(
                      builder: (context) {
                        return const MidiMappingPage();
                      },
                    ),
                  ),
                );
              },
            ),
          ),
          Tooltip(
            message: 'WiFi Provisioning',
            child: IconButton(
              icon: const Icon(Icons.settings),
              key: const Key('wifi provisioning button'),
              onPressed: () {
                unawaited(
                  Navigator.push<ProvisioningPage>(
                    context,
                    MaterialPageRoute(
                      builder: (context) => const ProvisioningPage(),
                    ),
                  ),
                );
              },
            ),
          ),
          Container(width: 10),
        ],
      ),
      body: Center(
        child: Column(
          children: [
            Builder(
              builder: (context) {
                final model = context.read<PresetsServiceBase>();
                final state = context.watch<PresetsState>();

                return Presets(
                  createPreset: state.map(
                    loading: (_) => null,
                    ready: (_) => model.create,
                  ),
                  savePreset: state.map(
                    loading: (_) => null,
                    ready: (ready) =>
                        ready.isCurrentModified ? model.saveChanges : null,
                  ),
                  deletePreset: state.map(
                    loading: (_) => null,
                    ready: (ready) => switch (ready.selectedPreset) {
                      null => null,
                      final int id => () => model.delete(id),
                    },
                  ),
                  revertPreset: state.map(
                    loading: (_) => null,
                    ready: (ready) => switch (ready.selectedPreset) {
                      null => null,
                      final int id => () => model.select(id),
                    },
                  ),
                  selectPreset: state.map(
                    loading: (_) => null,
                    ready: (ready) =>
                        (selectedPreset) => model.select(selectedPreset.id),
                  ),
                  selectNextPreset: state.map(
                    loading: (value) => null,
                    ready: (ready) {
                      final index = ready.presets
                              .indexWhere((e) => e.id == ready.selectedPreset) +
                          1;
                      if (ready.presets.hasIndex(index)) {
                        return () => model.select(ready.presets[index].id);
                      }

                      return null;
                    },
                  ),
                  selectPreviousPreset: state.map(
                    loading: (value) => null,
                    ready: (ready) {
                      final index = ready.presets
                              .indexWhere((e) => e.id == ready.selectedPreset) -
                          1;
                      if (ready.presets.hasIndex(index)) {
                        return () => model.select(ready.presets[index].id);
                      }

                      return null;
                    },
                  ),
                  presets: state.map(
                    loading: (_) => null,
                    ready: (ready) => ready.presets,
                  ),
                  selectedPreset: state.map(
                    loading: (_) => null,
                    ready: (ready) =>
                        ready.presets.cast<PresetRecord?>().firstWhere(
                              (e) => e!.id == ready.selectedPreset,
                              orElse: () => null,
                            ),
                  ),
                );
              },
            ),
            const MidiLearnStatus(),
            const Spacer(),
            const Pedalboard(),
            const Spacer(),
          ],
        ),
      ),
      bottomNavigationBar: BottomAppBar(
        child: Padding(
          padding: const EdgeInsets.all(4),
          child: Row(
            children: [
              Tooltip(
                message: 'Input clipping',
                child: Icon(
                  Icons.input,
                  color: context.watch<AudioClippingService>().inputIsClipped
                      ? Colors.red
                      : null,
                ),
              ),
              const SizedBox(width: 8),
              Tooltip(
                message: 'Output clipping',
                child: Icon(
                  Icons.output,
                  color: context.watch<AudioClippingService>().outputIsClipped
                      ? Colors.red
                      : null,
                ),
              ),
              const Spacer(),
              // Same size as icons
              WebSocketStatus(
                key: const Key('websocket-status'),
                size: IconTheme.of(context).size ?? 24.0,
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class ProvisioningPage extends StatelessWidget {
  const ProvisioningPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('WiFi Provisioning'),
      ),
      body: WifiProvisioningScreen(),
    );
  }
}

extension _HasIndexEx<T> on List<T> {
  bool hasIndex(int index) {
    return index >= 0 && index < length;
  }
}
