import 'package:freezed_annotation/freezed_annotation.dart';

import 'models.dart';

part 'midi_learn_state.freezed.dart';

@freezed
class MidiLearnState with _$MidiLearnState {
  const factory MidiLearnState.idle(
      List<MapEntry<String, MidiMapping>>? duplicates) = _Idle;
  const factory MidiLearnState.waitForParameter() = _WaitForParameter;
  const factory MidiLearnState.waitForMidi(String id) = _WaitForMidi;
  const factory MidiLearnState.savingMapping() = _SavingMapping;
}
