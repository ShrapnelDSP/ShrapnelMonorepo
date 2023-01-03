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
import 'dart:convert';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:flutter/material.dart';
import 'package:flutter_state_notifier/flutter_state_notifier.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:logging/logging.dart';
import 'package:provider/provider.dart';

import 'audio_events.dart';
import 'json_websocket.dart';
import 'midi_mapping/model/midi_learn.dart';
import 'midi_mapping/model/midi_learn_state.dart';
import 'midi_mapping/model/models.dart';
import 'midi_mapping/model/service.dart';
import 'midi_mapping/view/midi_learn.dart';
import 'midi_mapping/view/midi_mapping.dart';
import 'parameter.dart';
import 'pedalboard.dart';
import 'robust_websocket.dart';
import 'util/uuid.dart';
import 'websocket_status.dart';
import 'wifi_provisioning.dart';

final _log = Logger('shrapnel.main');

String formatDateTime(DateTime t) {
  final builder = StringBuffer()
    ..write(t.hour.toString().padLeft(2, '0'))
    ..write(':')
    ..write(t.minute.toString().padLeft(2, '0'))
    ..write(':')
    ..write(t.second.toString().padLeft(2, '0'))
    ..write('.')
    ..write(t.millisecond.toString().padLeft(3, '0'));

  return builder.toString();
}

void main() {
  Logger.root.level = Level.INFO;
  Logger.root.onRecord.listen((record) {
    debugPrint(
      '${record.level.name.padLeft("WARNING".length)} '
      '${formatDateTime(record.time)} '
      '${record.loggerName}: '
      '${record.message}',
    );
  });

  GoogleFonts.config.allowRuntimeFetching = false;

  runApp(App());
}

class App extends StatelessWidget {
  App({
    super.key,
    RobustWebsocket? websocket,
    JsonWebsocket? jsonWebsocket,
    Uuid? uuid,
    WifiProvisioningService? provisioning,
  }) {
    this.websocket = websocket ??
        RobustWebsocket(
          uri: Uri.parse('http://guitar-dsp.local:8080/websocket'),
        );
    jsonWebsocket ??= JsonWebsocket(websocket: this.websocket);
    audioClippingService = AudioClippingService(websocket: jsonWebsocket);
    this.uuid = uuid ?? Uuid();
    this.provisioning = provisioning ??
        WifiProvisioningService(
          provisioningFactory: () {
            _log.info('Creating provisioning connection');
            return Provisioning(
              security: Security1(pop: 'abcd1234'),
              transport: TransportHTTP('guitar-dsp.local'),
            );
          },
        );

    midiMappingService = MidiMappingService(
      websocket: jsonWebsocket,
    );
    parameterService = ParameterService(websocket: this.websocket);
    midiLearnService = MidiLearnService(
      mappingService: midiMappingService,
      uuid: this.uuid,
    );

    // TODO would be nicer to use an interface dedicated for listening to the
    // parameter update events. This stream has all the outgoing JSON messages.
    parameterService.sink.stream
        .map<dynamic>(json.decode)
        .map((dynamic e) => e as Map<String, dynamic>)
        .where((e) => e['messageType'] == 'parameterUpdate')
        .map(AudioParameterDouble.fromJson)
        .map((e) => e.id)
        .listen(midiLearnService.parameterUpdated);

    jsonWebsocket.dataStream
        .where((e) => e['messageType'] == 'MidiMap::midi_message_received')
        .map(MidiApiMessage.fromJson)
        .listen(
          (m) => m.maybeWhen(
            midiMessageReceived: midiLearnService.midiMessageReceived,
            orElse: () => null,
          ),
        );
  }

  late final RobustWebsocket websocket;
  late final AudioClippingService audioClippingService;
  late final Uuid uuid;
  late final WifiProvisioningService provisioning;
  late final MidiLearnService midiLearnService;
  late final ParameterService parameterService;
  late final MidiMappingService midiMappingService;

  @override
  Widget build(BuildContext context) {
    return MultiProvider(
      providers: [
        StateNotifierProvider<MidiLearnService, MidiLearnState>.value(
          value: midiLearnService,
        ),
        ChangeNotifierProvider.value(value: websocket),
        ChangeNotifierProvider.value(value: provisioning),
        ChangeNotifierProvider.value(
          value: parameterService,
        ),
        ChangeNotifierProvider.value(
          value: midiMappingService,
        ),
        ChangeNotifierProvider.value(value: uuid),
        ChangeNotifierProvider.value(value: audioClippingService),
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
      ),
      home: const MyHomePage(title: 'ShrapnelDSP'),
      debugShowCheckedModeBanner: false,
    );
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
                        context.read<MidiMappingService>().getMapping();
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
          children: const [
            MidiLearnStatus(),
            Spacer(),
            Pedalboard(),
            Spacer(),
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
              WebSocketStatus(size: IconTheme.of(context).size ?? 24.0),
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
