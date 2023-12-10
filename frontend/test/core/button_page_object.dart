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
