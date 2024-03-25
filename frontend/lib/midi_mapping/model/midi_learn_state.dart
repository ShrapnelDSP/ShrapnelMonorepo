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

import 'package:freezed_annotation/freezed_annotation.dart';

import 'models.dart';

part 'midi_learn_state.freezed.dart';

@freezed
class MidiLearnState with _$MidiLearnState {
  const factory MidiLearnState.loading() = _Loading;

  const factory MidiLearnState.idle(
    List<MapEntry<MidiMappingId, MidiMapping>>? duplicates,
  ) = _Idle;

  const factory MidiLearnState.waitForParameter() = _WaitForParameter;

  const factory MidiLearnState.waitForMidi(String id) = _WaitForMidi;

  const factory MidiLearnState.savingMapping() = _SavingMapping;
}
