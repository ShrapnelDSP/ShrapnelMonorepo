import 'package:flutter/cupertino.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/amplifier.dart';

import 'midi_mapping/midi_mapping_page_object.dart';

class HomePageObject {
  const HomePageObject(this.tester);

  final WidgetTester tester;

  Finder findKnob(String parameterId) => find.byKey(Key('knob-$parameterId'));

  Finder findMidiLearnWaitingForParameterMessage() =>
      find.textContaining('Move a knob');
  Finder findMidiLearnWaitingForMidiMessage() =>
      find.textContaining('Move a MIDI controller');

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
      warnIfMissed: false,
    );
    await tester.pumpAndSettle();
  }

  Future<void> toggleCollapsedStompbox(String name) async {
    await tester.tap(find.byKey(Key('stompbox-$name')));
    await tester.pumpAndSettle();
  }

  Future<void> toggleCollapsedAmplifier() async {
    await tester.tap(find.byType(Amplifier));
    await tester.pumpAndSettle();
  }
}
