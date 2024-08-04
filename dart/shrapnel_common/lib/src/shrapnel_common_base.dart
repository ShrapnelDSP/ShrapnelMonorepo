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
