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

@Timeout(Duration(seconds: 60))
@Tags(['api'])
library;

import 'dart:io';

import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:shrapnel/api/api_websocket.dart';
import 'package:shrapnel/core/message_transport.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/robust_websocket.dart';

import 'util.dart';

// audio parameter basic test
// factory reset
// all parameters loaded correctly
// update a parameter
// force save to NVS
// reboot
// check parameter reloaded

// preset test
// start up
// create preset
// change a parameter
// create another preset
// reboot
// last saved preset is reloaded
// load first preset
// parameter is restored to value stored in preset

// preset reload test
// start up
// create preset
// change a parameter
// load preset
// parameter is restored to value stored in preset

// MIDI mapping test
// for each mapping type
// start up
// create MIDI mapping
// trigger midi mapping
// verify correct reaction

// MIDI mapping preset
// Initial conditions:
//   preset exists
//   current parameters are different to preset
// start up
// create MIDI mapping
// trigger midi mapping
// verify correct reaction

// stress test
// factory reset
// do lots of stuff
// dump task list, heap stats

// Audio
// bypass all audio processing
// check level, gain, distortion etc

// Startup time test
// Measure startup time from when reset released to when API is usable

// ignore: do_not_use_environment
const dutIpAddress = String.fromEnvironment('DUT_IP_ADDRESS');
// ignore: do_not_use_environment
const networkSsid = String.fromEnvironment('NETWORK_SSID');
// ignore: do_not_use_environment
const networkPassphrase = String.fromEnvironment('NETWORK_PASSPHRASE');
// ignore: do_not_use_environment
const firmwareBinaryPath = String.fromEnvironment('FIRMWARE_BINARY_PATH');
const port = '/dev/ttyUSB0';

void main() {
  assert(dutIpAddress.isNotEmpty, 'DUT_IP_ADDRESS must be set');
  setupLogger(Level.ALL);

  late final MacAddress macAddress;
  late ApiWebsocket api;
  late ShrapnelUart uart;

  setUpAll(() async {
    macAddress = await flashFirmware(
      port: port,
      appPartitionAddress: 0x10000,
      path: firmwareBinaryPath,
    );
  });

  test('wifi provisioning', () async {
    await nvsErase(port: port);
    await connectToDutAccessPoint(macAddress);
    await setUpWiFi(ssid: networkSsid, password: networkPassphrase);
  });

  group('wifi already set up', () {
    setUp(() async {
      await nvsErase(port: port);

      uart = await ShrapnelUart.open('/dev/ttyUSB0');
      addTearDown(uart.dispose);

      await uart.provisionWifi(ssid: networkSsid, password: networkPassphrase);

      // Wait for firmware to start server after getting provisioned
      await Future<void>.delayed(const Duration(seconds: 10));

      const uri = 'ws://$dutIpAddress:8080/websocket';
      // closed by the WebSocketTransport
      // ignore: close_sinks
      final webSocket = await WebSocket.connect(uri);
      final webSocketTransport =
          WebSocketTransportAdapter(websocket: webSocket);
      api = ApiWebsocket(websocket: webSocketTransport);
      addTearDown(webSocketTransport.dispose);
    });

    test('simple test', () async {
      final transport = ParameterTransport(websocket: api);
      final service = ParameterService(transport: transport);

      final ampGain = AudioParameterDoubleModel(
        groupName: 'test',
        id: 'ampGain',
        name: 'Amp Gain',
        parameterService: service,
      );

      // Skip 1, because it's the default injected by the parameter model. We
      // are waiting for the first update from the firmware here.
      final update = await ampGain.value.skip(1).first;
      // Expect notification including the default value
      expect(update, closeTo(0.5, 0.000001));
    });
  });
}

/// Implements reconnecting feature for the low level WebSockets transport.
///
/// It is somewhat fake. New connections are never created and the instance is
/// always connected.
class WebSocketTransportAdapter extends WebSocketTransport
    implements ReconnectingMessageTransport<WebSocketData, WebSocketData> {
  WebSocketTransportAdapter({required super.websocket}) : super(onDone: () {});

  @override
  Stream<void> get connectionStream => const Stream.empty();

  @override
  // This class is always connected. It is constructed with an already connected
  // instance of the WebSockets connection
  bool get isAlive => true;
}
