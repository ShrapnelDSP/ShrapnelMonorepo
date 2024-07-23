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

import 'package:flutter/foundation.dart';
import 'package:flutter_test/flutter_test.dart';

class WifiProvisioningPageObject {
  WifiProvisioningPageObject(this.tester);

  final WidgetTester tester;

  Finder get findSuccessPage => find.textContaining('success');

  Finder get findScanCompletePage =>
      find.byKey(const Key('wifi scan complete page'));

  Future<void> startProvisioning() async {
    await tester.tap(find.byKey(const Key('wifi provisioning start')));
    await tester.pumpAndSettle();
  }

  Future<void> openAdvancedSetup() async {
    await tester.tap(find.textContaining('Advanced'));
    await tester.pumpAndSettle();
  }

  Future<void> enterSsid(String ssid) async {
    final ssidField = find.byKey(const Key('ssid text field'));
    await tester.enterText(ssidField, ssid);
  }

  Future<void> enterPassword(String password) async {
    final passwordField = find.byKey(const Key('password text field'));
    await tester.enterText(passwordField, password);
  }

  Future<void> submitPassword() async {
    await tester.tap(find.byKey(const Key('password submit button')));
  }

  Future<void> submitAdvanced() async {
    await tester.tap(find.byKey(const Key('advanced submit button')));
  }
}
