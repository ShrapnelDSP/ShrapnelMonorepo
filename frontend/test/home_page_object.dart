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

import 'dart:async';

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:logging/logging.dart';
import 'package:shrapnel/amplifier.dart';
import 'package:shrapnel/knob.dart';

import 'midi_mapping/midi_mapping_page_object.dart';
import 'presets/presets_page_object.dart';

final _log = Logger('home_page_object');

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

  bool get isConnected {
    final widget = find
        .descendant(
          of: find.byKey(const Key('websocket-status')),
          matching: find.byType(Tooltip),
        )
        .evaluate()
        .single
        .widget as Tooltip;

    _log.info(widget.message);

    return widget.message == 'Connected';
  }

  Future<void> waitUntilConnected(
      {Duration timeout = const Duration(seconds: 10)}) async {
    var keepGoing = true;
    final timer = Timer(timeout, () {
      _log.warning('Connection to provisioned access point timed out');
      keepGoing = false;
    });
    while (keepGoing) {
      if (isConnected) {
        timer.cancel();
        return;
      }
      await tester.pump(const Duration(seconds: 1));
    }

    throw TimeoutException('Connection timed out');
  }

  Future<void> createPreset(String name) async {
    await tester.tap(find.byKey(const Key('presets-create-button')));
    await tester.pumpAndSettle();

    final createPresetPage = CreatePresetPageObject(tester);
    await createPresetPage.submitName(name);
  }
}

class CreatePresetPageObject {
  CreatePresetPageObject(this.tester);

  final WidgetTester tester;

  Future<void> submitName(String name) async {
    await tester.enterText(find.byKey(const Key('presets-name-field')), name);

    await tester.tap(find.byKey(const Key('presets-confirm-name-button')));
    await tester.pumpAndSettle();
  }
}
