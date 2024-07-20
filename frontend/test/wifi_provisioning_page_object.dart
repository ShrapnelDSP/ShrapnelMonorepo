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
