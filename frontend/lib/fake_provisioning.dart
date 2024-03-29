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

import 'dart:math';
import 'dart:typed_data';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
// ignore: implementation_imports
import 'package:esp_softap_provisioning/src/connection_models.dart';

class FakeProvisioning implements Provisioning {
  final ssidHint = <String>[
    'fail at sendWifiConfig',
    'fail at applyWifiConfig',
    'fail at getStatus with null return',
    'fail at getStatus with AuthError',
    'fail at getStatus with NetworkNotFound',
    'fail at getStatus with connection timeout',
  ];

  late int _selectedSsidIndex;

  @override
  Future<bool> applyWifiConfig() {
    if (_selectedSsidIndex == 1) {
      return Future.value(false);
    }
    return Future.delayed(const Duration(milliseconds: 1000), () => true);
  }

  @override
  Future<bool> establishSession() =>
      Future.delayed(const Duration(milliseconds: 500), () => true);

  @override
  Future<ConnectionStatus?> getStatus() {
    ConnectionStatus? out;
    if (_selectedSsidIndex == 2) {
      out = null;
    } else if (_selectedSsidIndex == 3) {
      out = ConnectionStatus(
        state: WifiConnectionState.ConnectionFailed,
        failedReason: WifiConnectFailedReason.AuthError,
      );
    } else if (_selectedSsidIndex == 4) {
      out = ConnectionStatus(
        state: WifiConnectionState.ConnectionFailed,
        failedReason: WifiConnectFailedReason.NetworkNotFound,
      );
    } else if (_selectedSsidIndex == 5) {
      out = ConnectionStatus(state: WifiConnectionState.Connecting);
    }
    return Future.delayed(const Duration(milliseconds: 500), () => out);
  }

  @override
  Future<Uint8List> sendReceiveCustomData(
    Uint8List data, {
    int packageSize = 256,
  }) {
    throw UnimplementedError();
  }

  @override
  Future<bool> sendWifiConfig({String? ssid, String? password}) {
    _selectedSsidIndex = int.parse(ssid![5]);
    if (_selectedSsidIndex == 0) {
      return Future.value(false);
    }

    return Future.delayed(const Duration(milliseconds: 1000), () => true);
  }

  @override
  Future<List<Map<String, dynamic>>?> startScanWiFi() => Future.delayed(
        const Duration(milliseconds: 500),
        () => <Map<String, dynamic>>[
          _createFakeWifi(0),
          _createFakeWifi(1),
          _createFakeWifi(2),
          _createFakeWifi(3),
          _createFakeWifi(4),
          _createFakeWifi(5),
        ],
      );

  Map<String, dynamic> _createFakeWifi(int id) => <String, dynamic>{
        'ssid': 'SSID $id, ${id < ssidHint.length ? ssidHint[id] : ''}',
        'channel': id,
        'rssi': -60 - 5 * id,
        'bssid': _createFakeMac(id),
        // TODO This is not a realistic value for the auth field. What are the
        //      actual possible values?
        'auth': 'wpa',
      };

  List<int> _createFakeMac(int id) {
    final rand = Random(id);
    final mac = <int>[];

    for (var i = 0; i < 6; i++) {
      mac.add(rand.nextInt(256));
    }

    return mac;
  }

  // Security and transport should be treated as private. They are never
  // intialised, so any usage will throw.
  @override
  late Security security;

  @override
  late Transport transport;

  @override
  Future<void> dispose() {
    return Future<void>.value();
  }
}
