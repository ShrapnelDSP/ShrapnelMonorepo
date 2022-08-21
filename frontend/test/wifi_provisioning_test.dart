import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';
import 'package:shrapnel/fake_provisioning.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/wifi_provisioning.dart';

void main() {
  testWidgets('WiFi Provisioning page can be opened', (tester) async {
    final sut = MultiProvider(
      providers: [
        ChangeNotifierProvider(
          create: (context) => WifiProvisioningProvider(
            provisioningFactory: FakeProvisioning.new,
          ),
        ),
      ],
      child: const MaterialApp(
        home: ProvisioningPage(),
      ),
    );

    await tester.pumpWidget(sut);
    await tester.pumpAndSettle();
  });
}
