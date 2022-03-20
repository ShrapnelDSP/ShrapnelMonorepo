import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'robust_websocket.dart';

class WebSocketStatus extends StatelessWidget {
  const WebSocketStatus({Key? key, required this.size}) : super(key: key);

  final double size;

  @override
  Widget build(BuildContext context) {
    final isConnected = context.watch<RobustWebsocket>().isAlive;

    return Tooltip(
      message: isConnected ? 'Connected' : 'Not Connected',
      waitDuration: const Duration(milliseconds: 500),
      child: Container(
        width: size,
        height: size,
        decoration: BoxDecoration(
            color: isConnected ? Colors.green : Colors.red,
            shape: BoxShape.circle),
      ),
    );
  }
}
