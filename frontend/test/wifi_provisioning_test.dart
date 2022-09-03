import 'dart:math';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/wifi_provisioning.dart';

import 'wifi_provisioning_test.mocks.dart';

Map<String, dynamic> _createFakeWifi({
  required String ssid,
  required int channel,
  required int rssi,
  required List<int> mac,
  required String auth,
}) =>
    <String, dynamic>{
      'ssid': ssid,
      'channel': channel,
      'rssi': rssi,
      'bssid': mac,
      'auth': auth,
    };

@GenerateNiceMocks([MockSpec<RobustWebsocket>()])
@GenerateMocks([Provisioning])
void main() {
  late Widget sut;
  late MockProvisioning mockProvisioning;
  Function()? provisioningFactory;
  MockProvisioning provisioningFactoryWrapper() {
    provisioningFactory!.call();
    return mockProvisioning;
  }

  setUp(() async {
    sut = MultiProvider(
      providers: [
        ChangeNotifierProvider<RobustWebsocket>(
          create: (_) => MockRobustWebsocket(),
        ),
        ChangeNotifierProvider(
          create: (context) => WifiProvisioningProvider(
            provisioningFactory: provisioningFactoryWrapper,
          ),
        ),
      ],
      child: const MyApp(),
    );

    final fontData = rootBundle.load('google_fonts/NotoSans-Medium.ttf');
    final fontLoader = FontLoader('Noto Sans')..addFont(fontData);
    await fontLoader.load();
  });

  testWidgets('WiFi provisioning warns if already connected to device',
      (tester) async {
    // Set up fake websocket to look like we are alredy connected to the
    // ShrapnelDSP device, which means WiFi was already provisionined
    // successfully.

    // Expect that instead of the provisioning page openening, a message
    // appears explaining to the user that there is nothing to do, the device
    // is already provisionined.
  }, skip: true);

  testWidgets('WiFi provisioning can succeed', (tester) async {
    await tester.pumpWidget(sut);

    await tester.tap(find.byKey(const Key('wifi provisioning button')));
    await tester.pumpAndSettle();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession())
          .thenAnswer((_) => Future.delayed(const Duration(milliseconds: 500), () => true));
      when(mockProvisioning.startScanWiFi()).thenAnswer((_) => Future.value([
            _createFakeWifi(
              ssid: 'test SSID',
              channel: 5,
              rssi: -70,
              mac: [0x50, 0x51, 0x52, 0x53, 0x54, 0x55],
              auth: 'wpa',
            ),
            _createFakeWifi(
              ssid: 'test SSID',
              channel: 6,
              rssi: -71,
              mac: [0x60, 0x61, 0x62, 0x63, 0x64, 0x65],
              auth: 'wpa',
            ),
          ]));
    };

    await tester.tap(find.byKey(const Key('wifi provisioning start')));
    final count = await tester.pumpAndSettle();
    debugPrint('pumped $count times');

    await tester.pump(const Duration(seconds: 1));

    final ssidCard = find.textContaining('test SSID');
    expect(ssidCard, findsOneWidget);

    await tester.tap(ssidCard.first);
    await tester.pumpAndSettle();

  });

  testWidgets('WiFi provisioning fails if not connected to access point',
      (tester) async {
    await tester.pumpWidget(sut);

    await tester.tap(find.byKey(const Key('wifi provisioning button')));
    await tester.pumpAndSettle();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession())
          .thenAnswer((_) => Future.value(false));
    };

    await tester.tap(find.byKey(const Key('wifi provisioning start')));
    final count = await tester.pumpAndSettle();
    debugPrint('pumped $count times');

    expect(find.textContaining('failed'), findsOneWidget);
  });
}
