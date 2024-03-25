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
import 'package:flutter_riverpod/flutter_riverpod.dart' as riverpod;
import 'package:google_fonts/google_fonts.dart';
import 'package:logging/logging.dart';

import 'audio_events.dart';
import 'midi_mapping/model/midi_learn.dart';
import 'midi_mapping/view/midi_learn.dart';
import 'midi_mapping/view/midi_mapping.dart';
import 'pedalboard.dart';
import 'presets/model/presets.dart';
import 'presets/model/presets_service.dart';
import 'presets/view/presets.dart';
import 'provider_observer.dart';
import 'status/view/websocket_status.dart';
import 'wifi_provisioning.dart';

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

  runApp(
    riverpod.ProviderScope(
      observers: [
        // ignore: do_not_use_environment
        if (const bool.fromEnvironment('ENABLE_PROVIDER_OBSERVER'))
          ProviderObserverImpl(),
      ],
      child: App(),
    ),
  );
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

class App extends riverpod.ConsumerWidget {
  App({super.key});

  @override
  Widget build(BuildContext context, riverpod.WidgetRef ref) {
    return const MyApp();
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

class MyHomePage extends riverpod.ConsumerWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  Widget build(BuildContext context, riverpod.WidgetRef ref) {
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
        actions: [
          Tooltip(
            message: 'MIDI Learn',
            child: IconButton(
              icon: const Icon(Icons.menu_book_outlined),
              key: const Key('midi-learn-button'),
              onPressed: ref.watch(midiLearnServiceProvider).maybeWhen(
                    loading: () => null,
                    orElse: () => () => startMidiLearning(ref),
                  ),
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
                final model = ref.read(presetsServiceProvider.notifier);
                final state = ref.watch(presetsServiceProvider);

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
                      final int id => () => unawaited(model.delete(id)),
                    },
                  ),
                  revertPreset: state.map(
                    loading: (_) => null,
                    ready: (ready) => switch (ready.selectedPreset) {
                      null => null,
                      final int id => () => unawaited(model.select(id)),
                    },
                  ),
                  selectPreset: state.map(
                    loading: (_) => null,
                    ready: (ready) => (selectedPreset) =>
                        unawaited(model.select(selectedPreset.id)),
                  ),
                  selectNextPreset: state.map(
                    loading: (value) => null,
                    ready: (ready) {
                      final index = ready.presets
                              .indexWhere((e) => e.id == ready.selectedPreset) +
                          1;
                      if (ready.presets.hasIndex(index)) {
                        return () =>
                            unawaited(model.select(ready.presets[index].id));
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
                        return () =>
                            unawaited(model.select(ready.presets[index].id));
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
                  color:
                      ref.watch(audioClippingServiceProvider)?.inputIsClipped ??
                              false
                          ? Colors.red
                          : null,
                ),
              ),
              const SizedBox(width: 8),
              Tooltip(
                message: 'Output clipping',
                child: Icon(
                  Icons.output,
                  color: ref
                              .watch(audioClippingServiceProvider)
                              ?.outputIsClipped ??
                          false
                      ? Colors.red
                      : null,
                ),
              ),
              const Spacer(),
              // Same size as icons
              WebSocketStatus(
                size: IconTheme.of(context).size ?? 24.0,
              ),
            ],
          ),
        ),
      ),
    );
  }

  void startMidiLearning(riverpod.WidgetRef ref) {
    ref.read(midiLearnServiceProvider.notifier).startLearning();
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
