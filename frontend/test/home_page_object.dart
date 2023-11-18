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

import 'package:flutter/cupertino.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/amplifier.dart';
import 'package:shrapnel/knob.dart';

import 'midi_mapping/midi_mapping_page_object.dart';
import 'presets/presets_page_object.dart';

class HomePageObject {
  HomePageObject(this.tester);

  final WidgetTester tester;

  Finder findKnob(String parameterId) => find.byKey(Key('knob-$parameterId'));

  Finder findMidiLearnWaitingForParameterMessage() =>
      find.textContaining('Move a knob');

  Finder findMidiLearnWaitingForMidiMessage() =>
      find.textContaining('Move a MIDI controller');

  Finder findDuplicateMappingSnackbar() =>
      find.textContaining('Some duplicate mappings were deleted');

  Finder findDuplicateMappingUndoButton() =>
      find.byKey(const Key('undo-remove-duplicate-mappings'));

  Future<void> startMidiLearn() async {
    await tester.tap(find.byKey(const Key('midi-learn-button')));
    await tester.pumpAndSettle();
  }

  Future<MidiMappingPageObject> openMidiMapping() async {
    await tester.tap(find.byKey(const Key('midi-mapping-button')));
    await tester.pumpAndSettle();
    return MidiMappingPageObject(tester);
  }

  Future<void> dragKnob({required String parameterId}) async {
    await tester.drag(
      findKnob(parameterId),
      const Offset(5 * kDragSlopDefault, 0),
    );
    await tester.pumpAndSettle();
  }

  double getKnobValue({required String parameterId}) {
    final knob = findKnob(parameterId).evaluate().single.widget as Knob;
    return knob.value;
  }

  Future<void> toggleCollapsedStompbox(String name) async {
    await tester.tap(find.byKey(Key('stompbox-$name')));
    await tester.pumpAndSettle();
  }

  Future<void> toggleCollapsedAmplifier() async {
    await tester.tap(find.byType(Amplifier));
    await tester.pumpAndSettle();
  }

  Future<void> restoreRemovedDuplicateMappings() async {
    await tester.tap(findDuplicateMappingUndoButton());
    await tester.pumpAndSettle();
  }

  late final _presetsPage = PresetsPageObject(tester);
  PresetsPageObject get presetsPage => _presetsPage;
}
