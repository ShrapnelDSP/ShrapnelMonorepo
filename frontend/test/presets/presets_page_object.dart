import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

import '../core/button_page_object.dart';

class PresetsPageObject {
  PresetsPageObject(this.tester);

  final WidgetTester tester;

  void getCurrentPresetName() => find.descendant(
        of: find.byKey(Key('presets-current-preset-name')),
        matching: find.byType(Text),
      );

  late ButtonPageObject saveButton =
      ButtonPageObject(tester, key: Key("presets-save-button"));
}
