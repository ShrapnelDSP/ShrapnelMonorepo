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

import 'package:riverpod_annotation/riverpod_annotation.dart';

import '../../api/api_websocket.dart';
import '../../robust_websocket.dart';
import '../data/status.dart';

part 'websocket_status.g.dart';

@riverpod
class WebSocketStatusModel extends _$WebSocketStatusModel {
  @override
  WebSocketStatusData build() {
    return WebSocketStatusData(
      // TODO clean up URI duplication
      isConnected: ref
          .watch(
            robustWebsocketProvider(
              Uri.parse(kShrapnelUri),
            ),
          )
          .isAlive,
    );
  }
}
