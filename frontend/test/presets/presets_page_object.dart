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
    final dropdown = find
        .byKey(const Key('presets-current-preset-name'))
        .evaluate()
        .single
        .widget as DropdownButton<PresetState>;
    return dropdown.value!.name;
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
}
