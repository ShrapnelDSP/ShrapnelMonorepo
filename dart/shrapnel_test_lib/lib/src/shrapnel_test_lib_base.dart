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
import 'dart:io';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:flutter_libserialport/flutter_libserialport.dart';
import 'package:logging/logging.dart';
import 'package:path/path.dart' as p;
import 'package:shrapnel_common/api_data/midi_api_message.dart';
import 'package:shrapnel_common/api_data/proto_extension.dart';
import 'package:shrapnel_common/shrapnel_common.dart';

typedef MacAddress = List<int>;

final _log = Logger('firmware_api_test_util');

Future<void> connectToDutAccessPoint(MacAddress macAddress) async {
  final dutAccessPointSsid =
      'shrapnel_${macAddress.sublist(3).map((e) => e.toRadixString(16)).join().toUpperCase()}';
  final result = await Process.run(
    'nmcli',
    'connection up $dutAccessPointSsid'.split(' '),
  );

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);
}

Future<void> setUpWiFi({required String ssid, required String password}) async {
  final log = Logger('setUpWiFi');

  log.info('setting up Wi-Fi using SSID: $ssid');

  final provisioning = await _retry(() async {
    final provisioning = Provisioning(
      security: Security1(pop: 'abcd1234'),
      transport: TransportHTTP('192.168.4.1'),
    );

    final success = await provisioning.establishSession();
    if (!success) {
      throw Exception('provisioning failed to establish session');
    }

    return provisioning;
  });

  var success =
  await provisioning.sendWifiConfig(ssid: ssid, password: password);
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

Future<void> _eraseFlash({required String port}) async {
  _log.info('Flashing firmware');

  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = ['-p', port, 'erase_flash'];

  final result = await Process.run(command, args);

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);
}

List<int> _getMacAddressFromEsptoolStdout(String espToolStdout) {
  final macRegex =
  RegExp(r'^MAC: (([0-9a-f]{2}:){5}([0-9a-f]{2}))$', multiLine: true);
  final macString = macRegex.allMatches(espToolStdout).single.group(1)!;
  return macString.split(':').map((e) => int.parse(e, radix: 16)).toList();
}

/// Load the firmware to the device.
///
/// If the firmware is already loaded, then does nothing and quickly returns.
Future<MacAddress> flashFirmware({
  required String port,
  required int appPartitionAddress,
  required String path,
}) async {
  // If already flashed, skip flash erase and firmware download. This is
  // important to make local development have fast iteration. Erase and download
  // both take around 10 seconds, verifying that the firmware is already loaded
  // takes about 1 second.

  final (isAlreadyFlashed, macAddress) = await _checkIfAlreadyFlashed(
    port: port,
    appPartitionAddress: appPartitionAddress,
    binaryFilePath: p.join(path, 'esp32-dsp.bin'),
  );

  if (!isAlreadyFlashed) {
    await _eraseFlash(port: port);
    await _flashFirmware(port: port, path);
  } else {
    _log.info('Firmware appears to be already flashed, skip flashing');
  }

  return macAddress;
}

Future<(bool, MacAddress)> _checkIfAlreadyFlashed({
  required String port,
  required int appPartitionAddress,
  required String binaryFilePath,
}) async {
  // The app image header [1] contains a few key pieces of information that
  // allows us to determine if the firmware needs to be flashed or not.
  //
  // The version field changes when the CMake provided app version, or the
  // esp-idf autogenerated (based on git describe) version changes. This is most
  // likely for builds triggered in a CI context. This number wouldn't change
  // development when a local checkout is being modified for debugging.
  //
  // The app_elf_sha256 field changes any time the firmware is modified or
  // rebuild (because esp-idf builds are not reproducible by default). This is
  // what triggers the flashing in local development use case.
  //
  // [1]: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/app_image_format.html

  // total size of binary headers and app image header including the elf file
  // sha256
  const headerSize = 208;

  final expectedHeader = File(binaryFilePath).openSync().readSync(headerSize);
  if (expectedHeader.length != headerSize) {
    throw StateError('Header read from file is too small.');
  }

  final tmpPath = Directory.systemTemp.createTempSync('shrapneldsp_test');
  final expectedHeaderFilePath = p.join(tmpPath.path, 'expected_header.bin');

  File(expectedHeaderFilePath).writeAsBytesSync(expectedHeader, flush: true);

  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = '-p $port '
      '-b 2000000 '
      '--before default_reset --after hard_reset '
      '--chip esp32 '
      'verify_flash 0x10000 $expectedHeaderFilePath'
      .split(' ');

  final result = await Process.run(command, args);

  tmpPath.deleteSync(recursive: true);

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);

  return (
  switch (result.exitCode) {
    0 => true,
    2 => false,
    _ => throw StateError(
      'esptool.py exited with unexpected code ${result.exitCode}',
    ),
  },
  _getMacAddressFromEsptoolStdout(result.stdout as String)
  );
}

Future<void> _flashFirmware(String path, {required String port}) async {
  _log.info('Flashing firmware');

  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = '-p $port '
      '-b 2000000 '
      '--before default_reset --after hard_reset '
      '--chip esp32 '
      'write_flash --flash_mode dio --flash_size 4MB --flash_freq 80m '
      '0x1000 $path/bootloader.bin '
      '0x8000 $path/partition-table.bin '
      '0x10000 $path/esp32-dsp.bin'
      .split(' ');

  final result = await Process.run(command, args);

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);
}

Future<void> nvsErase({required String port}) async {
  _log.info('Erasing NVS partition');

  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = '-p $port '
      '-b 2000000 '
      '--before default_reset --after hard_reset '
      '--chip esp32 '
      'erase_region 0x9000 0x6000'
      .split(' ');

  final result = await Process.run(command, args);

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);
}

Future<void> nvsLoad(String binaryPath, {required String port}) async {
  _log.info('Flashing NVS partition');

  // pip can be used to install esptool into the global environment:
  // https://docs.espressif.com/projects/esptool/en/latest/esp32/#quick-start
  const command = 'esptool.py';
  final args = '-p $port '
      '-b 2000000 '
      '--before default_reset --after hard_reset '
      '--chip esp32 '
      'write_flash --flash_mode dio --flash_size 4MB --flash_freq 80m '
      '0x9000 $binaryPath'
      .split(' ');

  final result = await Process.run(command, args);

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);
}

/// UART driver
class ShrapnelUart {
  ShrapnelUart._(this.port, this.reader) {
    log.listen(_logger.info);
  }

  static final _logger = Logger('ShrapnelUart');

  SerialPort port;
  SerialPortReader reader;

  static Future<ShrapnelUart> open(String name) async {
    final port = SerialPort(name);
    final reader = SerialPortReader(port);

    final success = port.openReadWrite();
    if (!success) {
      throw StateError('serial port failed to open');
    }

    port.config = SerialPortConfig()
      ..baudRate = 2000000
      ..bits = 8
      ..parity = 0
      ..stopBits = 1
      ..setFlowControl(SerialPortFlowControl.none);

    return ShrapnelUart._(port, reader);
  }

  Future<void> resetFirmware() async {
    // TODO use DTR/RTS
  }

  Future<void> sendMidiMessage(MidiMessage message) async {
    final encoded = base64Encode(message.toProto().writeToBuffer());

    final command = 'midi -d $encoded';

    final bytesToWrite = ascii.encode('$command\n');
    final bytesWritten = port.write(bytesToWrite);

    if (bytesToWrite.length != bytesWritten) {
      throw StateError('UART write failed');
    }
  }

  Future<void> provisionWifi({
    required String ssid,
    required String password,
  }) async {
    final command =
        'wifi ${escapeCommandArg(ssid)} ${escapeCommandArg(password)}';

    final bytesToWrite = ascii.encode('$command\n');
    final bytesWritten = port.write(bytesToWrite);

    if (bytesToWrite.length != bytesWritten) {
      throw StateError('UART write failed');
    }
  }

  late final _log = reader.stream
      .cast<List<int>>()
      .transform<String>(utf8.decoder)
      .transform(const LineSplitter())
      .asBroadcastStream();

  Stream<String> get log => _log;

  void dispose() {
    port.flush();
    reader.close();
    port.close();
    port.dispose();
  }
}

String escapeCommandArg(String arg) {
  return arg.replaceAllMapped(
    RegExp(r'[\\" ]'),
        (match) => '\\${match.group(0)!}',
  );
}

Future<T> _retry<T>(Future<T> Function() fn) async {
  try {
    return await fn();
  } on Exception catch (_) {
    _log.warning('retrying');
    return fn();
  }
}
