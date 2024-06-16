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
import 'package:shrapnel/api/proto_extension.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';
import 'package:shrapnel/wifi_provisioning.dart';

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

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);

  final macRegex =
      RegExp(r'^MAC: (([0-9a-f]{2}:){5}([0-9a-f]{2}))$', multiLine: true);
  final macString =
      macRegex.allMatches(result.stdout as String).single.group(1)!;
  return macString.split(':').map((e) => int.parse(e, radix: 16)).toList();
}

Future<void> flashFirmware(String path) async {
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

  _log.info(result.exitCode);
  _log.info(result.pid);
  _log.info(result.stderr);
  _log.info(result.stdout);
}

/// UART driver
class ShrapnelUart {
  ShrapnelUart._(this.port, this.reader);

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

  late final _log = reader.stream
      .cast<List<int>>()
      .transform<String>(utf8.decoder)
      .transform(const LineSplitter());

  Stream<String> get log => _log;

  void dispose() {
    port.flush();
    reader.close();
    port.close();
    port.dispose();
  }
}

class FakeMidi {
  FakeMidi(this.uart);

  ShrapnelUart uart;

  Future<void> sendMessage(MidiMessage message) async {
    await uart.sendMidiMessage(message);
  }
}
