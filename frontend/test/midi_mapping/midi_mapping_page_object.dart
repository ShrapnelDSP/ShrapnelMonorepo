import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';
import 'package:shrapnel/midi_mapping/view/midi_mapping.dart';

class MidiMappingPageObject {
  const MidiMappingPageObject(this.tester);

  final WidgetTester tester;

  Finder findMappingRows() => find.byType(MidiChannelDropdown);
  Finder findPage() => find.text('MIDI Mapping');

  Future<MidiMappingCreatePageObject> openCreateDialog() async {
    await tester.tap(find.byType(FloatingActionButton));
    await tester.pumpAndSettle();
    return MidiMappingCreatePageObject(tester);
  }

  Future<void> updateMidiChannel({
    required String id,
    required int value,
  }) async {
    await tester.tap(find.byKey(Key('$id-midi-channel-dropdown')).last);
    await tester.pumpAndSettle();
    await tester.tap(find.text('$value').last);
    await tester.pumpAndSettle();
  }

  Future<void> updateCcNumber({
    required String id,
    required int value,
  }) async {
    await tester.tap(find.byKey(Key('$id-cc-number-dropdown')).last);
    await tester.pumpAndSettle();
    await tester.tap(find.text('$value').last);
    await tester.pumpAndSettle();
  }

  Future<void> updateMode({
    required String id,
    required MidiMappingMode value,
  }) async {
    await tester.tap(find.byKey(Key('$id-mode-dropdown')).last);
    await tester.pumpAndSettle();
    await tester.tap(find.text(value.uiName).last);
    await tester.pumpAndSettle();
  }

  Future<void> updateParameter({
    required String id,
    required String value,
  }) async {
    final dropdown = find.byKey(Key('$id-parameter-id-dropdown')).last;
    await tester.scrollUntilVisible(dropdown, 50);
    await tester.tap(dropdown);
    await tester.pumpAndSettle();
    await tester.tap(find.text(value).last);
    await tester.pumpAndSettle();
  }

  Future<void> deleteMapping({required String id}) async {
    await tester.tap(find.byKey(Key('$id-delete-button')).last);
    await tester.pumpAndSettle();
  }
}

class MidiMappingCreatePageObject {
  const MidiMappingCreatePageObject(this.tester);

  final WidgetTester tester;

  Future<void> selectMidiChannel(int channel) async {
    await tester.tap(
      find.ancestor(
        of: find.text('MIDI channel'),
        matching: find.byType(DropdownButton<int>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text('$channel').last);
    await tester.pumpAndSettle();
  }

  Future<void> selectCcNumber(int ccNumber) async {
    await tester.tap(
      find.ancestor(
        of: find.text('CC number'),
        matching: find.byType(DropdownButton<int>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text('$ccNumber').last);
    await tester.pumpAndSettle();
  }

  Future<void> selectMode(MidiMappingMode mode) async {
    await tester.tap(
      find.ancestor(
        of: find.text('Mode'),
        matching: find.byType(DropdownButton<MidiMappingMode>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text(mode.uiName).last);
    await tester.pumpAndSettle();
  }

  Future<void> selectParameter(String name) async {
    await tester.tap(
      find.ancestor(
        of: find.text('Parameter'),
        matching: find.byType(DropdownButton<String>),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text(name).last);
    await tester.pumpAndSettle();
  }

  Future<void> submitCreateDialog() async {
    await tester.tap(find.text('Create'));
    await tester.pumpAndSettle();
  }
}
