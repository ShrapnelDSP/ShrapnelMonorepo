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

import 'dart:async';

import 'package:flutter_test/flutter_test.dart';

Future<bool> pumpWaitingFor({
  required WidgetTester tester,
  required bool Function() predicate,
  required Duration timeout,
}) async {
  var keepGoing = true;
  final timer = Timer(timeout, () {
    keepGoing = false;
  });
  while (keepGoing) {
    if (predicate()) {
      timer.cancel();
      return true;
    }
    await tester.pump(const Duration(seconds: 1));
  }

  return false;
}
