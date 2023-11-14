import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/presets/model/presets.dart';

import '../core/button_page_object.dart';

class PresetsPageObject {
  PresetsPageObject(this.tester);

  final WidgetTester tester;

  String getCurrentPresetName() {
    /*
    return (find
          .descendant(
            of: find.byKey(const Key('presets-current-preset-name')),
            matching: find.byType(Text),
          )
          .evaluate()
          .single
          .widget as Text)
      .data!;
     */
    final dropdown = find
        .byKey(const Key('presets-current-preset-name'))
        .evaluate()
        .single
        .widget as DropdownButton<PresetState>;
    return dropdown.value!.name;
  }

  late final ButtonPageObject saveButton =
      ButtonPageObject(tester, key: const Key('presets-save-button'));

  late final ButtonPageObject createButton =
      ButtonPageObject(tester, key: const Key('presets-create-button'));
}
