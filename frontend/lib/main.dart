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

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:logging/logging.dart';
import 'package:provider/provider.dart';

import 'parameter.dart';
import 'pedalboard.dart';
import 'robust_websocket.dart';
import 'websocket_status.dart';
import 'wifi_provisioning.dart';

final log = Logger('main');

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
  Logger.root.level = Level.ALL;
  Logger.root.onRecord.listen((record) {
    debugPrint(
        '${record.level.name.padLeft("WARNING".length)} ${formatDateTime(record.time)} ${record.loggerName}: ${record.message}');
  });

  GoogleFonts.config.allowRuntimeFetching = false;

  runApp(MultiProvider(
    providers: [
      ChangeNotifierProvider(
          create: (_) => RobustWebsocket(
              uri: Uri.parse('http://guitar-dsp.local:8080/websocket'))),
      ChangeNotifierProvider(
          create: (_) => WifiProvisioningProvider(provisioningFactory: () {
                log.info('Creating provisioning connection');
                return Provisioning(
                  security: Security1(pop: 'abcd1234'),
                  transport: TransportHTTP('guitar-dsp.local'),
                );
              })),
    ],
    child: const MyApp(),
  ));
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ShrapnelDSP',
      theme: ThemeData(
        brightness: Brightness.dark,
        primarySwatch: Colors.orange,
        fontFamily: 'Noto Sans',
      ),
      home: const MyHomePage(title: 'ShrapnelDSP'),
      debugShowCheckedModeBanner: false,
    );
  }
}

class MyHomePage extends StatelessWidget {
  const MyHomePage({Key? key, required this.title}) : super(key: key);
  final String title;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
        actions: [
          Container(
            margin: const EdgeInsets.all(10),
            child: IconButton(
              icon: const Icon(Icons.settings),
              key: const Key('wifi provisioning button'),
              onPressed: () {
                Navigator.push<ProvisioningPage>(
                  context,
                  MaterialPageRoute(
                      builder: (context) => const ProvisioningPage()),
                );
              },
            ),
          ),
          const WebSocketStatus(size: kToolbarHeight - 20),
          Container(width: 10),
        ],
      ),
      body: const ParameterServiceProvider(
        child: Center(
          child: Pedalboard(),
        ),
      ),
    );
  }
}

class ProvisioningPage extends StatelessWidget {
  const ProvisioningPage({Key? key}) : super(key: key);

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
