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

import 'dart:async';

import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';
import 'package:shrapnel/api/api_websocket.dart';
import 'package:shrapnel/core/stream_extensions.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/provider_observer.dart';

import '../riverpod_util.dart';
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

  setUpAll(() async {
    macAddress = await flashFirmware(
      port: port,
      appPartitionAddress: 0x10000,
      path: firmwareBinaryPath,
    );
  });

  test(skip: true, 'wifi provisioning', () async {
    await nvsErase(port: port);
    await connectToDutAccessPoint(macAddress);
    await setUpWiFi(ssid: networkSsid, password: networkPassphrase);
  });

  group('wifi already set up', () {
    setUp(() async {
      await nvsErase(port: port);

      final uart = await ShrapnelUart.open('/dev/ttyUSB0');
      addTearDown(uart.dispose);

      await uart.provisionWifi(ssid: networkSsid, password: networkPassphrase);

      // Wait for firmware to start server after getting provisioned
      await Future<void>.delayed(const Duration(seconds: 10));
    });

    test('simple test', () async {
      final log = Logger('debug');

      log.shout('create container');
      final container = createContainer(
        overrides: [
          normalHostProvider.overrideWithValue(dutIpAddress),
        ],
        observers: [ProviderObserverImpl()],
      );

      /*
      final stream = container
          .listenAsStream(audioParameterDoubleModelProvider('ampGain'))
          .logInfo(
            Logger('debug'),
            (event) => 'event $event',
          );

      log.shout('setup expectation');
      final expectation = expectLater(
        stream,
        emitsInOrder([
          isAsyncLoading<double>(),
          isAsyncData<double>(closeTo(0.5, 0.000001)),
        ]),
      );

      log.shout('pump1');
      await pumpEventQueue();
      await Future<void>.delayed(const Duration(seconds: 1));

      log.shout('pump2');
      await pumpEventQueue();
      await Future<void>.delayed(const Duration(seconds: 1));

      log.shout('pump3');
      await pumpEventQueue();

      log.shout('wait expectation');
      await Future<void>.delayed(const Duration(seconds: 10));
      await expectation;
      */

      container.listen(audioParameterDoubleModelProvider('ampGain'),
          (previous, next) {
        Logger('debug').info('prev $previous next $next');
      });

      /*
      container
          .listenAsStream(
            audioParameterDoubleModelProvider('ampGain'),
          )
          .listen((event) => Logger('debug').info('event $event'));
       */

      await Future<void>.delayed(const Duration(seconds: 10));
    });
  });
}

extension ProviderContainerTestEx on ProviderContainer {
  Stream<T> listenAsStream<T>(ProviderListenable<T> provider) {
    final controller = StreamController<T>();

    Logger('debug').shout('listening');
    late final subscription = listen(
      provider,
      (o, value) {
        Logger('debug').shout('provider notified $o $value');
        controller.add(value);
      },
      fireImmediately: false,
    );

    controller.onCancel = () {
      subscription.close();
      unawaited(controller.close());
    };

    return controller.stream;
  }
}

Matcher isAsyncLoading<T>() => isA<AsyncLoading<T>>();

Matcher isAsyncData<T>(Matcher matcher) =>
    isA<AsyncData<T>>().having((data) => data.value, 'value', matcher);
