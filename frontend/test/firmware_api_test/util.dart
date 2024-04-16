import 'dart:io';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:logging/logging.dart';
import 'package:shrapnel/wifi_provisioning.dart';

typedef MacAddress = List<int>;

Future<void> connectToDutAccessPoint(MacAddress macAddress) async {
  final dutAccessPointSsid =
      'shrapnel_${macAddress.sublist(3).map((e) => e.toRadixString(16)).join().toUpperCase()}';
  final result = await Process.run(
    'nmcli',
    'connection up $dutAccessPointSsid'.split(' '),
  );

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

Future<MacAddress> eraseFlash() async {
  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = ['erase_flash'];

  final result = await Process.run(command, args);

  print(result.exitCode);
  print(result.pid);
  print(result.stderr);
  print(result.stdout);

  final macRegex =
      RegExp(r'^MAC: (([0-9a-f]{2}:){5}([0-9a-f]{2}))$', multiLine: true);
  final macString =
      macRegex.allMatches(result.stdout as String).single.group(1)!;
  return macString.split(':').map((e) => int.parse(e, radix: 16)).toList();
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
