import 'package:freezed_annotation/freezed_annotation.dart';

part 'status.freezed.dart';

@freezed
class WebSocketStatusData with _$WebSocketStatusData  {
  const factory WebSocketStatusData({required bool isConnected}) = _WebSocketStatusData;
}