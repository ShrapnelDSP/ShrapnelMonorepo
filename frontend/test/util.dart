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
