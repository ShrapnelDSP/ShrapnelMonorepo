import 'package:freezed_annotation/freezed_annotation.dart';

part 'audio_event_message.freezed.dart';

@freezed
sealed class AudioEventMessage with _$AudioEventMessage {
  factory AudioEventMessage.inputClipped() = AudioEventMessageInputClipped;

  factory AudioEventMessage.outputClipped() = AudioEventMessageOutputClipped;
}
