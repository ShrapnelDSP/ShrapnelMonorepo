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
