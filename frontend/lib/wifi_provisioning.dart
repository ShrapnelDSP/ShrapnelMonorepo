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
import 'package:logging/logging.dart';
import 'package:provider/provider.dart';

class _Strings {
  static const initialMessage =
      'Please connect to the ShrapnelDSP WiFi access point (PROV_XXX).';
  static const initialButtonText = 'Ready';
  static const connectingMessage = 'Connecting...';
  static const sessionFailureMesssage =
      'Connection failed. Please ensure you are connected to the ShrapnelDSP '
      'access point and try again.';
  static const sessionFailureButtonText = 'Try again';
}

final log = Logger('robust_websocket');

class WifiProvisioningScreen extends StatelessWidget {
  WifiProvisioningScreen({Key? key}) : super(key: key);

  Widget buildInitial(BuildContext context, void Function() onReadyPressed) {
    return Center(
        child: Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Container(
          margin: const EdgeInsets.all(10),
          child: const Text(_Strings.initialMessage),
        ),
        ElevatedButton(
          onPressed: onReadyPressed,
          child: const Text(_Strings.initialButtonText),
        ),
      ],
    ));
  }

  Widget buildConnecting(BuildContext context) {
    return Center(
        child: Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Container(
          margin: const EdgeInsets.all(10),
          child: const Text(_Strings.connectingMessage),
        ),
      ],
    ));
  }

  Widget buildSessionFailure(
    BuildContext context,
    void Function() onReadyPressed,
  ) {
    return Center(
        child: Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Container(
          margin: const EdgeInsets.all(10),
          child: const Text(_Strings.sessionFailureMesssage),
        ),
        ElevatedButton(
          onPressed: onReadyPressed,
          child: const Text(_Strings.sessionFailureButtonText),
        ),
      ],
    ));
  }

  Widget buildWifiScanning(
      BuildContext context, WifiProvisioningProvider provisioning) {
    final accessPointCount = provisioning.accessPoints?.length ?? 0;

    return Column(children: [
      if (accessPointCount == 0) const Text('Scanning...'),
      if (accessPointCount != 0)
        Expanded(
          child: ListView.builder(
            shrinkWrap: true,
            itemCount: accessPointCount,
            itemBuilder: (context, index) =>
                Text('${provisioning.accessPoints?[index]['ssid']}'),
          ),
        ),
    ]);
  }

  @override
  Widget build(BuildContext context) {
    return Consumer<WifiProvisioningProvider>(builder: (_, provisioning, __) {
      Widget child;
      switch (provisioning.state) {
        case WifiProvisioningState.initial:
          child = buildInitial(context, provisioning.start);
          break;
        case WifiProvisioningState.connecting:
          child = buildConnecting(context);
          break;
        case WifiProvisioningState.sessionFailure:
          child = buildSessionFailure(context, provisioning.start);
          break;
        case WifiProvisioningState.scanning:
          child = buildWifiScanning(context, provisioning);
          break;
        default:
          child = Text(provisioning.state.toString());
          break;
      }

      return WillPopScope(
        onWillPop: () {
          provisioning.reset();
          return Future.value(true);
        },
        child: child,
      );
    });
  }
}

enum WifiProvisioningState {
  initial,
  connecting,
  sessionFailure,
  scanning,
  testing,
  failure,
  success,
}

class WifiProvisioningProvider extends ChangeNotifier {
  WifiProvisioningProvider({required this.provisioning});

  ProvisioningBase provisioning;

  var _statePrivate = WifiProvisioningState.initial;

  List<Map<String, dynamic>>? _accessPointsPrivate;

  set _accessPoints(List<Map<String, dynamic>>? newAccessPoints) {
    _accessPointsPrivate = newAccessPoints;
    notifyListeners();
  }

  List<Map<String, dynamic>>? get _accessPoints => _accessPointsPrivate;
  List<Map<String, dynamic>>? get accessPoints => _accessPoints;

  set _state(WifiProvisioningState newState) {
    _statePrivate = newState;
    log.fine('state: $_statePrivate');
    notifyListeners();
  }

  WifiProvisioningState get _state => _statePrivate;
  WifiProvisioningState get state => _state;

  /// Connect to device and start provisioning process
  Future<void> start() async {
    log.info('started');
    if (state != WifiProvisioningState.initial &&
        state != WifiProvisioningState.sessionFailure) {
      throw StateError('start called in unexpected state ${_state.toString()}');
    }

    _state = WifiProvisioningState.connecting;
    final isConnected = await provisioning.establishSession();
    if (_state != WifiProvisioningState.connecting) {
      log.info('establishSession cancelled');
      return;
    }

    log.info('scanning');
    if (isConnected) {
      _state = WifiProvisioningState.scanning;
      final accessPoints = await provisioning.startScanWiFi();
      if (_state != WifiProvisioningState.scanning) {
        log.info('scanning cancelled');
        return;
      }
      _accessPoints = accessPoints;
    } else {
      _state = WifiProvisioningState.sessionFailure;
    }
  }

  void reset() {
    _state = WifiProvisioningState.initial;
  }
}
