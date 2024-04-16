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
    final macAddress = await eraseFlash();
    await flashFirmware();
    await connectToDutAccessPoint(macAddress);
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
