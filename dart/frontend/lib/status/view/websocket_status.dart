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

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../data/status.dart';

class WebSocketStatus extends StatelessWidget {
  const WebSocketStatus({super.key, required this.size});

  final double size;

  @override
  Widget build(BuildContext context) {
    final isConnected = context.watch<WebSocketStatusData>().isConnected;

    return Tooltip(
      message: isConnected ? 'Connected' : 'Not Connected',
      waitDuration: const Duration(milliseconds: 500),
      child: Container(
        width: size,
        height: size,
        decoration: BoxDecoration(
          color: isConnected ? Colors.green : Colors.red,
          shape: BoxShape.circle,
        ),
      ),
    );
  }
}
