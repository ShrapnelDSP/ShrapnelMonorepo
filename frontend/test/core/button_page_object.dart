import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

// sealed class _ButtonKind {}

/// A page object representing a tappable button such as [IconButton],
/// [TextButton] etc.
class ButtonPageObject {
  ButtonPageObject(this.tester, {required this.key});

  final WidgetTester tester;
  final Key key;
  late final finder = find.byKey(key);

   get isEnabled => finder.first.evaluate().single.widget;

   void press() {

   }

   void longPress() {

   }

   // _ButtonKind _getWidget()
}