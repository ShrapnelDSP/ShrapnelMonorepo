import 'package:state_notifier/state_notifier.dart';

import '../../robust_websocket.dart';
import '../data/status.dart';

class WebSocketStatusModel extends StateNotifier<WebSocketStatusData> {
  WebSocketStatusModel({required this.websocket})
      : super(const WebSocketStatusData(isConnected: false)) {
    websocket.addListener(_onStateChanged);
  }

  void _onStateChanged() {
    state = WebSocketStatusData(isConnected: websocket.isAlive);
  }

  final RobustWebsocket websocket;

  @override
  void dispose() {
    super.dispose();
    websocket.removeListener(_onStateChanged);
  }
}
