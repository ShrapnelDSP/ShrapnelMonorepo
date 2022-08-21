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
      'Please connect to the ShrapnelDSP WiFi access point (shrapnel_XXXXXX).';
  static const initialButtonText = 'Ready';
  static const connectingMessage = 'Connecting...';
  static const testingMessage = 'Testing...';
  static const sessionFailureMesssage =
      'Connection failed. Please ensure you are connected to the ShrapnelDSP '
      'access point.';
  static const retryButtonText = 'Try again';
  static const failureMessage = 'The ShrapnelDSP device failed to connect to '
      'your WiFi access point.';
  // TODO how to add the SSID to this message?
  // https://stackoverflow.com/questions/52278035/flutter-internationalization-dynamic-strings
  // https://github.com/dart-lang/sdk/issues/1694
  static const failureReasonMessage = {
    WifiConnectFailedReason.AuthError: 'The password provided was incorrect.',
    WifiConnectFailedReason.NetworkNotFound: 'The network was not found.',
  };
  static const wifiPasswordSubmitButtonText = 'Join';
}

final log = Logger('wifi_provisioning');

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
        padding: const EdgeInsets.all(8),
        itemCount: accessPointCount,
        itemBuilder: (context, index) {
          final ssid = provisioning.accessPoints![index]['ssid'] as String;
          final bssid = provisioning.accessPoints![index]['bssid'] as List<int>;
          final bssidFormatted = <String>[];

          for (final byte in bssid) {
            bssidFormatted
                .add(byte.toRadixString(16).padLeft(2, '0').toUpperCase());
          }

          final bssidString = bssidFormatted.join(':');

          return Card(
            child: ListTile(
              title: Text(ssid),
              subtitle: Text('BSSID: $bssidString\n'
                  'Security: ${provisioning.accessPoints![index]['auth']}'),
              // TODO there are supposed to be wifi_2_bar and wifi_1_bar icons
              //      too, but these are missing from the Icons class for some
              //      reason
              trailing: Tooltip(
                message:
                    'RSSI: ${provisioning.accessPoints![index]['rssi'] as int}',
                child: Icon(
                    provisioning.accessPoints![index]['rssi'] as int > -65
                        ? Icons.wifi
                        : Icons.signal_wifi_0_bar),
              ),
              onTap: () {
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
          );
        },
      );
    }

    return Center(
      child: child,
    );
  }
}

class WifiProvisioningScreen extends StatelessWidget {
  WifiProvisioningScreen({Key? key}) : super(key: key);

  Widget buildInitial(BuildContext context) {
    final provisioning =
        Provider.of<WifiProvisioningProvider>(context, listen: false);

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
          onPressed: provisioning.start,
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

  Widget buildTesting(BuildContext context) {
    return Center(
        child: Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Container(
          margin: const EdgeInsets.all(10),
          child: const Text(_Strings.testingMessage),
        ),
      ],
    ));
  }

  Widget buildSessionFailure(BuildContext context) {
    final provisioning =
        Provider.of<WifiProvisioningProvider>(context, listen: false);

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
          onPressed: provisioning.start,
          child: const Text(_Strings.retryButtonText),
        ),
      ],
    ));
  }

  Widget buildFailure(BuildContext context) {
    final provisioning = Provider.of<WifiProvisioningProvider>(context);

    return Center(
        child: Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Container(
          margin: const EdgeInsets.all(10),
          child: Text(
              provisioning.status?.failedReason == null
                  ? _Strings.failureMessage
                  : '${_Strings.failureMessage} ${_Strings.failureReasonMessage[provisioning.status!.failedReason]!}',
              textAlign: TextAlign.center),
        ),
        ElevatedButton(
          onPressed: provisioning.start,
          child: const Text(_Strings.retryButtonText),
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
          child = buildInitial(context);
          break;
        case WifiProvisioningState.connecting:
          child = buildConnecting(context);
          break;
        case WifiProvisioningState.sessionFailure:
          child = buildSessionFailure(context);
          break;
        case WifiProvisioningState.scanning:
          child = _WifiScanningScreen();
          break;
        case WifiProvisioningState.testing:
          child = buildTesting(context);
          break;
        case WifiProvisioningState.failure:
          child = buildFailure(context);
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
  WifiProvisioningProvider({required this.provisioningFactory});

  /// Will be called to construct an instance of [Provisioning]
  Provisioning Function() provisioningFactory;

  Provisioning? provisioning;

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
        state != WifiProvisioningState.sessionFailure &&
        state != WifiProvisioningState.failure) {
      throw StateError('start called in unexpected state ${_state.toString()}');
    }

    reset();

    provisioning = provisioningFactory();

    _state = WifiProvisioningState.connecting;
    final isConnected = await provisioning!.establishSession();
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
    // TODO need to remove duplicate SSID here
    final accessPoints = await provisioning!.startScanWiFi();

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
        await provisioning!.sendWifiConfig(ssid: ssid, password: passphrase);
    if (!success) {
      log.severe('send wifi config failed');
      _state = WifiProvisioningState.sessionFailure;
      return;
    }

    log.info('apply wifi');
    success = await provisioning!.applyWifiConfig();
    if (!success) {
      log.severe('apply wifi config failed');
      _state = WifiProvisioningState.sessionFailure;
      return;
    }

    _status = await _pollWifiStatusWithTimeout(const Duration(seconds: 10));
    if (!_status.isSuccess()) {
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
      log.warning('Connection to provisioned access point timed out');
      keepGoing = false;
    });
    ConnectionStatus? status;

    while (keepGoing) {
      status = await provisioning!.getStatus();

      if (status.isTerminal()) {
        timer.cancel();
        return status;
      }
      await Future<void>.delayed(const Duration(seconds: 1));
    }

    return status;
  }

  void reset() {
    _accessPoints = null;
    selectedAccessPoint = null;
    provisioning = null;
    _status = null;
    _state = WifiProvisioningState.initial;
  }
}

extension ShrapnelAdditions on ConnectionStatus? {
  bool isTerminal() {
    if (this == null) {
      return true;
    }

    final state = this!.state;

    return state == WifiConnectionState.Connected ||
        state == WifiConnectionState.Disconnected ||
        state == WifiConnectionState.ConnectionFailed;
  }

  bool isSuccess() {
    if (this == null) {
      return false;
    }

    return this!.state == WifiConnectionState.Connected;
  }
}
