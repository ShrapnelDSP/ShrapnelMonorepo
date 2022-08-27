import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:provider/provider.dart';
import 'package:shrapnel/fake_provisioning.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/wifi_provisioning.dart';

import 'wifi_provisioning_test.mocks.dart';

@GenerateNiceMocks([MockSpec<RobustWebsocket>()])
void main() {
  late Widget sut;

  setUp(() async {
    sut = MultiProvider(
      providers: [
      ChangeNotifierProvider<RobustWebsocket>(
          create: (_) => MockRobustWebsocket()),
        ChangeNotifierProvider(
          create: (context) => WifiProvisioningProvider(
            provisioningFactory: FakeProvisioning.new,
          ),
        ),
      ],
      child: const MyApp(),
    );

      final fontData = rootBundle.load('google_fonts/NotoSans-Medium.ttf');
      final fontLoader = FontLoader('Noto Sans')..addFont(fontData);
      await fontLoader.load();
  });

  testWidgets('WiFi Provisioning page can be opened', (tester) async {
    await tester.pumpWidget(sut);

    await expectLater(find.byType(MyApp),
            matchesGoldenFile('home.png'));

    await tester.tap(find.byKey(const Key('wifi provisioning button')));
    await tester.pumpAndSettle();

    await expectLater(find.byType(MyApp),
            matchesGoldenFile('provisioning.png'));

    await tester.tap(find.byKey(const Key('wifi provisioning start')));
    final count = await tester.pumpAndSettle();
    debugPrint('pumped $count times');

    await expectLater(find.byType(MyApp),
            matchesGoldenFile('connecting.png'));

    await tester.pump(const Duration(seconds: 1));

    await expectLater(find.byType(MyApp),
            matchesGoldenFile('next.png'));

  });
}
