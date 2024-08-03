import 'dart:async';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
// ignore: implementation_imports
import 'package:esp_softap_provisioning/src/connection_models.dart';
import 'package:logging/logging.dart';

final _log = Logger('shrapnel.common');

extension StatusPollEx on Provisioning {
  Future<ConnectionStatus?> pollStatus(Duration timeout) async {
    var keepGoing = true;
    final timer = Timer(timeout, () {
      _log.warning('Connection to provisioned access point timed out');
      keepGoing = false;
    });
    ConnectionStatus? status;

    while (keepGoing) {
      status = await getStatus();

      if (status.isTerminal()) {
        timer.cancel();
        return status;
      }
      await Future<void>.delayed(const Duration(seconds: 1));
    }

    return status;
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
