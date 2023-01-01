import 'package:freezed_annotation/freezed_annotation.dart';

part 'midi_learn_state.freezed.dart';

@freezed
class MidiLearnState with _$MidiLearnState {
  const factory MidiLearnState.idle() = _Idle;
  const factory MidiLearnState.waitForParameter() = _WaitForParameter;
  const factory MidiLearnState.waitForMidi(String id) = _WaitForMidi;
}

