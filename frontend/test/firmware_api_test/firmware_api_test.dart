@Tags(['api'])
library;

import 'dart:io';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:shrapnel/api/api_websocket.dart';
import 'package:shrapnel/core/message_transport.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/wifi_provisioning.dart';

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
const networkSsid = String.fromEnvironment('NETWORK_SSID');
const networkPassphrase = String.fromEnvironment('NETWORK_PASSPHRASE');

// Initial implementation
//
// Assumes firmware already flashed and running. Later it will build and flash
// the firmware.
//
// Make a single request and expect a response.
void main() {
  assert(dutIpAddress.isNotEmpty, 'DUT_IP_ADDRESS must be set');
  setupLogger(Level.ALL);

  late ApiWebsocket api;

  setUp(() async {
    await eraseFlash();
    await flashFirmware();
    await connectToDutAccessPoint();
    await setUpWiFi(ssid: networkSsid, password: networkPassphrase);

    // Wait for firmware to start server after getting provisioned
    await Future<void>.delayed(const Duration(seconds: 10));

    const uri = 'ws://$dutIpAddress:8080/websocket';
    // closed by the WebSocketTransport
    // ignore: close_sinks
    final webSocket = await WebSocket.connect(uri);
    final webSocketTransport = WebSocketTransportAdapter(websocket: webSocket);
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

Future<void> connectToDutAccessPoint() async {
  // TODO inject the MAC address
  final result =
      await Process.run('nmcli', 'connection up shrapnel_17124D'.split(' '));

  print(result.exitCode);
  print(result.pid);
  print(result.stderr);
  print(result.stdout);
}

Future<void> setUpWiFi({required String ssid, required String password}) async {
  final log = Logger('setUpWiFi');

  log.info('setting up Wi-Fi using SSID: $ssid');

  final provisioning = Provisioning(
    security: Security1(pop: 'abcd1234'),
    transport: TransportHTTP('guitar-dsp.local'),
  );

  var success = await provisioning.establishSession();
  if (!success) {
    throw StateError('provisioning failed to establish session');
  }

  success = await provisioning.sendWifiConfig(ssid: ssid, password: password);
  if (!success) {
    throw StateError('send wifi config failed');
  }

  success = await provisioning.applyWifiConfig();
  if (!success) {
    throw StateError('apply wifi config failed');
  }

  final status = await provisioning.pollStatus(const Duration(seconds: 10));
  if (!status.isSuccess()) {
    throw StateError('connection to provisioned network failed: $status');
  }
}

Future<void> eraseFlash() async {
  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = ['erase_flash'];

  final result = await Process.run(command, args);

  print(result.exitCode);
  print(result.pid);
  print(result.stderr);
  print(result.stdout);
}

Future<void> flashFirmware() async {
  // TODO pass in path using --dart-define
  const path = '../firmware/dist';

  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = '-b 2000000 '
          '--before default_reset --after hard_reset '
          '--chip esp32 '
          'write_flash --flash_mode dio --flash_size 4MB --flash_freq 80m '
          '0x1000 $path/bootloader.bin '
          '0x8000 $path/partition-table.bin '
          '0x10000 $path/esp32-dsp.bin'
      .split(' ');

  final result = await Process.run(command, args);

  print(result.exitCode);
  print(result.pid);
  print(result.stderr);
  print(result.stdout);
}
