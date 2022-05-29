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
import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
// ignore: implementation_imports
import 'package:esp_softap_provisioning/src/connection_models.dart';
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
  static const wifiPasswordSubmitButtonText = 'Join';
}

final log = Logger('robust_websocket');

class _WifiPasswordDialog extends StatefulWidget {
  @override
  State<_WifiPasswordDialog> createState() => _WifiPasswordDialogState();
}

class _WifiPasswordDialogState extends State<_WifiPasswordDialog> {
  late TextEditingController _controller;

  bool _isObscure = true;

  @override
  void initState() {
    super.initState();
    _controller = TextEditingController();
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  void submitWifiPassword(BuildContext context) {
    final provisioning =
        Provider.of<WifiProvisioningProvider>(context, listen: false);
    provisioning.join(_controller.value.text);
    Navigator.pop(context);
  }

  @override
  Widget build(BuildContext context) => Consumer<WifiProvisioningProvider>(
        builder: (context, provisioning, _) {
          final screenWidth = MediaQuery.of(context).size.width;
          const dialogWidth = 300;
          var insets = (screenWidth - dialogWidth) / 2;
          if (insets < 8) {
            insets = 8;
          }

          return Dialog(
            insetPadding: EdgeInsets.symmetric(horizontal: insets),
            child: Padding(
              padding: const EdgeInsets.all(8),
              child: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                    provisioning
                            .accessPoints![provisioning.selectedAccessPoint!]
                        ['ssid'] as String,
                    style: Theme.of(context).textTheme.titleLarge,
                  ),
                  Container(height: 16),
                  TextField(
                    controller: _controller,
                    onSubmitted: (_) => submitWifiPassword(context),
                    obscureText: _isObscure,
                    decoration: InputDecoration(
                      border: const OutlineInputBorder(),
                      labelText: 'Password',
                      suffixIcon: IconButton(
                        icon: Icon(_isObscure
                            ? Icons.visibility
                            : Icons.visibility_off),
                        onPressed: () {
                          setState(() {
                            _isObscure = !_isObscure;
                          });
                        },
                      ),
                    ),
                  ),
                  Container(height: 8),
                  MaterialButton(
                    onPressed: () {
                      submitWifiPassword(context);
                    },
                    child: const Text(_Strings.wifiPasswordSubmitButtonText),
                  ),
                ],
              ),
            ),
          );
        },
      );
}

class _WifiScanningScreen extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => _WifiScanningScreenState();
}

class _WifiScanningScreenState extends State<_WifiScanningScreen> {
  @override
  Widget build(BuildContext context) {
    final provisioning = Provider.of<WifiProvisioningProvider>(context);

    final accessPointCount = provisioning.accessPoints?.length ?? 0;

    Widget child;

    if (accessPointCount == 0) {
      child = const Text('Scanning...');
    } else {
      child = ListView.builder(
        itemCount: accessPointCount,
        itemBuilder: (context, index) => Card(
          child: ListTile(
            title: Text('${provisioning.accessPoints![index]['ssid']}'),
            subtitle:
                Text('BSSID: ${provisioning.accessPoints![index]['bssid']}\n'
                    'Security: ${provisioning.accessPoints![index]['auth']}'),
            // TODO there are supposed to be wifi_2_bar and wifi_1_bar icons
            //      too, but these are missing from the Icons class for some
            //      reason
            trailing: Icon(
                provisioning.accessPoints![index]['rssi'] as int > -65
                    ? Icons.wifi
                    : Icons.signal_wifi_0_bar),
            onTap: () {
              // TODO Pass the selected SSID to a dialog
              //      Dialog asks for password and starts the provisioning
              //      process
              //      Once submitted, we show a provisioning screen, then the
              //      provisioning result once awailable

              provisioning.selectedAccessPoint = index;
              showDialog<void>(
                  context: context,
                  builder: (context) =>
                      ChangeNotifierProvider<WifiProvisioningProvider>.value(
                        value: provisioning,
                        child: _WifiPasswordDialog(),
                      ));
            },
          ),
        ),
      );
    }

    return Center(
      child: child,
    );
  }
}

class WifiProvisioningScreen extends StatelessWidget {
  WifiProvisioningScreen({Key? key}) : super(key: key);

  Widget buildInitial(BuildContext context, void Function() onReadyPressed) {
    return Center(
        child: Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Container(
          margin: const EdgeInsets.all(10),
          child:
              const Text(_Strings.initialMessage, textAlign: TextAlign.center),
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
          child: const Text(_Strings.sessionFailureMesssage,
              textAlign: TextAlign.center),
        ),
        ElevatedButton(
          onPressed: onReadyPressed,
          child: const Text(_Strings.sessionFailureButtonText),
        ),
      ],
    ));
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
          child = _WifiScanningScreen();
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
  // TODO This is used for all failures during the process, rename it and
  //      update strings to match
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

  ConnectionStatus? _statusPrivate;
  set _status(ConnectionStatus? newStatus) {
    _statusPrivate = newStatus;
    notifyListeners();
  }

  ConnectionStatus? get _status => _statusPrivate;
  ConnectionStatus? get status => _status;

  int? selectedAccessPoint;

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
    if (!isConnected) {
      log.severe('establish session failed');
      _state = WifiProvisioningState.sessionFailure;
      return;
    }

    _state = WifiProvisioningState.scanning;
    final accessPoints = await provisioning.startScanWiFi();

    if (_state != WifiProvisioningState.scanning) {
      log.info('scanning cancelled');
      return;
    }

    if (accessPoints == null) {
      log.severe('scanning failed');
      _state = WifiProvisioningState.sessionFailure;
      return;
    }

    _accessPoints = accessPoints;
  }

  // TODO cancel if reset is called
  Future<void> join(String passphrase) async {
    if (state != WifiProvisioningState.scanning) {
      throw StateError('join called in unexpected state ${_state.toString()}');
    }

    if (selectedAccessPoint == null) {
      throw StateError('join called when selectedAccessPoint is null');
    }

    if (selectedAccessPoint! < 0 ||
        selectedAccessPoint! > accessPoints!.length) {
      throw StateError(
          'join called when selectedAccessPoint has invalid value $selectedAccessPoint');
    }

    final ssid = accessPoints![selectedAccessPoint!]['ssid'] as String;

    _state = WifiProvisioningState.testing;
    log.info('send wifi');
    var success =
        await provisioning.sendWifiConfig(ssid: ssid, password: passphrase);
    if (!success) {
      log.severe('send wifi config failed');
      _state = WifiProvisioningState.sessionFailure;
      return;
    }

    log.info('apply wifi');
    success = await provisioning.applyWifiConfig();
    if (!success) {
      log.severe('apply wifi config failed');
      _state = WifiProvisioningState.sessionFailure;
    }

    _status = await _pollWifiStatusWithTimeout(const Duration(seconds: 10));
    if (!_isSuccessStatus(_status)) {
      log.severe('could not connect to provisioned access point '
          '${_status?.state.toString()} '
          '${_status?.failedReason?.toString()}');
      _state = WifiProvisioningState.failure;
      return;
    }

    _state = WifiProvisioningState.success;
    return;
  }

  Future<ConnectionStatus?> _pollWifiStatusWithTimeout(Duration timeout) async {
    var keepGoing = true;
    final timer = Timer(timeout, () {
      keepGoing = false;
    });
    ConnectionStatus? status;

    while (keepGoing) {
      status = await provisioning.getStatus();

      if (_isTerminalStatus(status)) {
        timer.cancel();
        return status;
      }
      await Future<void>.delayed(const Duration(seconds: 1));
    }

    return status;
  }

  bool _isTerminalStatus(ConnectionStatus? status) {
    return status == null ||
        status.state == WifiConnectionState.Connected ||
        status.state == WifiConnectionState.Disconnected ||
        status.state == WifiConnectionState.ConnectionFailed;
  }

  bool _isSuccessStatus(ConnectionStatus? status) {
    if (status == null) {
      return false;
    }

    return status.state == WifiConnectionState.Connected;
  }

  void reset() {
    _accessPoints = null;
    selectedAccessPoint = null;
    _state = WifiProvisioningState.initial;
  }
}
