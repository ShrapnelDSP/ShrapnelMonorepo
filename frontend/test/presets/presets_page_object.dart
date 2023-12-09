import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/presets/model/presets.dart';

import '../core/button_page_object.dart';

class PresetsPageObject {
  PresetsPageObject(this.tester);

  final WidgetTester tester;

  late final ButtonPageObject saveButton =
      ButtonPageObject(tester, key: const Key('presets-save-button'));

  late final ButtonPageObject createButton =
      ButtonPageObject(tester, key: const Key('presets-create-button'));

  String getCurrentPresetName() {
    return _dropdown.value!.preset.name;
  }

  Future<void> createPreset(String name) async {
    createButton.press();
    await tester.pumpAndSettle();

    final nameField = find.byKey(const Key('presets-name-field'));
    expect(nameField, findsOneWidget);

    await tester.enterText(nameField, name);
    final confirmButton =
        ButtonPageObject(tester, key: const Key('presets-confirm-name-button'));
    confirmButton.press();

    await tester.pumpAndSettle();
  }

  Future<void> selectPreset(String name) async {
    await tester.tap(find.byKey(const Key('presets-current-preset-name')));
    await tester.pumpAndSettle();
    await tester.tap(find.text(name).last);
    await tester.pumpAndSettle();
  }

  DropdownButton<PresetRecord> get _dropdown {
    return find
        .byKey(const Key('presets-current-preset-name'))
        .evaluate()
        .single
        .widget as DropdownButton<PresetRecord>;
  }

}
