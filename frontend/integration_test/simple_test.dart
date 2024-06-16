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

import 'package:async/async.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:shrapnel/core/message_transport.dart';
import 'package:shrapnel/core/stream_extensions.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';
import 'package:shrapnel/robust_websocket.dart';

import '../test/firmware_api_test/util.dart';
import '../test/home_page_object.dart';

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

// Initial implementation
//
// Assumes firmware already flashed and running. Later it will build and flash
// the firmware.
//
// Make a single request and expect a response.
void main() {
  assert(dutIpAddress.isNotEmpty, 'DUT_IP_ADDRESS must be set');
  setupLogger(Level.ALL);

  setUpAll(() async {
    final macAddress = await flashFirmware(
      appPartitionAddress: 0x10000,
      path: firmwareBinaryPath,
    );

    // TODO add an option to load pre-provisioned wifi
    await nvsErase();

    await connectToDutAccessPoint(macAddress);
    await setUpWiFi(ssid: networkSsid, password: networkPassphrase);

    // TODO save the NVS partition after provisioning so it can be reloaded
    // between tests

    // Wait for firmware to start server after getting provisioned
    await Future<void>.delayed(const Duration(seconds: 10));
  });

  setUp(() async {
    // TODO reload the NVS partition created after provisioning

    // wait for firmware to start
  });

  testWidgets('uart console smoke', (tester) async {
    await tester.runAsync(() async {
      final uart = await ShrapnelUart.open('/dev/ttyUSB0');
      addTearDown(uart.dispose);

      // At least one response line has to arrive within 1 second
      final responseLogs = uart.log
          .logInfo(Logger('firmware'), (event) => event)
          .first
          .timeout(const Duration(seconds: 1));

      await uart.sendMidiMessage(
        const MidiMessage.controlChange(
          channel: 0,
          control: 1,
          value: 0x00,
        ),
      );

      await expectLater(responseLogs, completes);
    });
  });

  testWidgets('audio parameters', (tester) async {
    await tester.runAsync(() async {
      // audio parameter basic test
      // factory reset
      // all parameters loaded correctly
      // update a parameter
      // force save to NVS
      // reboot
      // check parameter reloaded
    });
  });

  testWidgets('simple test', (tester) async {
    await tester.pumpWidget(App());

    // wait until connected
    // poll connection status widget until ready with timeout
    final homePage = HomePageObject(tester);
    await homePage.waitUntilConnected();

    await homePage.createPreset('Preset 1');

    final midiMappingPage = await homePage.openMidiMapping();

    expect(midiMappingPage.findPage(), findsOneWidget);
    expect(midiMappingPage.findMappingRows(), findsNothing);

    // create a mapping
    final midiMappingCreatePage = await midiMappingPage.openCreateDialog();
    await midiMappingCreatePage.selectMidiChannel(1);
    await midiMappingCreatePage.selectCcNumber(2);
    await midiMappingCreatePage.selectMode(MidiMappingMode.parameter);
    // XXX: There is a bug in flutter where the DropdownButton's popup menu is
    //      unreliable during tests: https://github.com/flutter/flutter/issues/82908
    //
    // Pick an arbitrary parameter here. The only criteria for selection is that
    // it actually works during the test. This is more likely if something is
    // picked from the top of the list.
    await midiMappingCreatePage.selectParameter('Chorus: DEPTH');
    await midiMappingCreatePage.submitCreateDialog();

    await tester.pump(const Duration(seconds: 1));
    await tester.pumpAndSettle();

    // Expect new mapping visible in UI
    expect(midiMappingPage.findMappingRows(), findsOneWidget);

    await midiMappingPage.openCreateDialog();
    await midiMappingCreatePage.selectMidiChannel(2);
    await midiMappingCreatePage.selectCcNumber(3);
    await midiMappingCreatePage.selectMode(MidiMappingMode.button);
    await midiMappingCreatePage.selectPreset('Preset 1');
    await midiMappingCreatePage.submitCreateDialog();

    await tester.pump(const Duration(seconds: 1));
    await tester.pumpAndSettle();

    // Expect new mapping visible in UI
    expect(midiMappingPage.findMappingRows(), findsNWidgets(2));

    await midiMappingPage.openCreateDialog();
    await midiMappingCreatePage.selectMidiChannel(3);
    await midiMappingCreatePage.selectCcNumber(4);
    await midiMappingCreatePage.selectMode(MidiMappingMode.toggle);
    // XXX: There is a bug in flutter where the DropdownButton's popup menu is
    //      unreliable during tests: https://github.com/flutter/flutter/issues/82908
    //
    // Pick an arbitrary parameter here. The only criteria for selection is that
    // it actually works during the test. This is more likely if something is
    // picked from the top of the list.
    await midiMappingCreatePage.selectParameter('Chorus: RATE');
    await midiMappingCreatePage.submitCreateDialog();

    await tester.pump(const Duration(seconds: 1));
    await tester.pumpAndSettle();

    // Expect new mapping visible in UI
    expect(midiMappingPage.findMappingRows(), findsNWidgets(3));
  }, skip: true);
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
