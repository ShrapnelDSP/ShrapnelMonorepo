import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

sealed class _ButtonKind {}

class _IconButtonKind extends _ButtonKind {
  _IconButtonKind(this.button);

  IconButton button;
}

class _ButtonStyleButtonKind extends _ButtonKind {
  _ButtonStyleButtonKind(this.button);

  ButtonStyleButton button;
}

/// A page object representing a tappable button such as [IconButton],
/// [TextButton] etc.
class ButtonPageObject {
  ButtonPageObject(this.tester, {required this.key});

  final WidgetTester tester;
  final Key key;
  late final finder = find.byKey(key);

  bool get isEnabled => switch (_getWidget()) {
        final _IconButtonKind b => b.button.onPressed != null,
        final _ButtonStyleButtonKind b =>
          b.button.onPressed != null || b.button.onLongPress != null,
      };

  void press() {
    switch (_getWidget()) {
      case final _IconButtonKind b:
        b.button.onPressed?.call();
      case final _ButtonStyleButtonKind b:
        b.button.onPressed?.call();
    }
  }

  void longPress() {
    switch (_getWidget()) {
      case final _IconButtonKind b:
        b.button.onPressed?.call();
      case final _ButtonStyleButtonKind b:
        b.button.onLongPress?.call();
    }
  }

  _ButtonKind _getWidget() => switch (finder.first.evaluate().single.widget) {
        final IconButton button => _IconButtonKind(button),
        final ButtonStyleButton button => _ButtonStyleButtonKind(button),
        _ => throw StateError('Unsupported button type'),
      };
}
