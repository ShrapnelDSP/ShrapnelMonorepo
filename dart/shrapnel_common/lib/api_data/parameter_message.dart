import 'package:freezed_annotation/freezed_annotation.dart';

part 'parameter_message.freezed.dart';

@freezed
sealed class ParameterServiceOutputMessage
    with _$ParameterServiceOutputMessage {
  factory ParameterServiceOutputMessage.requestInitialisation() =
      ParameterServiceOutputMessageRequestInitialisation;

  factory ParameterServiceOutputMessage.parameterUpdate({
    required String id,
    required double value,
  }) = ParameterServiceOutputMessageParameterUpdate;
}

@freezed
sealed class ParameterServiceInputMessage with _$ParameterServiceInputMessage {
  factory ParameterServiceInputMessage.parameterUpdate({
    required String id,
    required double value,
  }) = ParameterServiceInputMessageParameterUpdate;
}
