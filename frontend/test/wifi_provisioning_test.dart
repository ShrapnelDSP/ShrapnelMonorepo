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

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
import 'package:esp_softap_provisioning/src/connection_models.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/api/api_websocket.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/wifi_provisioning.dart';

import 'home_page_object.dart';
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
  testWidgets(
    'WiFi provisioning warns if already connected to device',
    (tester) async {
      // Set up fake websocket to look like we are alredy connected to the
      // ShrapnelDSP device, which means WiFi was already provisionined
      // successfully.

      // Expect that instead of the provisioning page openening, a message
      // appears explaining to the user that there is nothing to do, the device
      // is already provisionined.
    },
    skip: true,
  );

  testWidgets(
    'WiFi provisioning can succeed',
    (tester) async {
      late MockProvisioning mockProvisioning;
      void Function()? provisioningFactory;
      MockProvisioning provisioningFactoryWrapper() {
        provisioningFactory!.call();
        return mockProvisioning;
      }

      final websocket = MockRobustWebsocket();

      await tester.pumpWidget(
        ProviderScope(
          overrides: [
            robustWebsocketProvider(kShrapnelUri)
                .overrideWith((_) => websocket),
            provisioningProvider.overrideWith(
              (_) => WifiProvisioningService(
                provisioningFactory: provisioningFactoryWrapper,
              ),
            ),
          ],
          child: App(),
        ),
      );

      final homePage = HomePageObject(tester);
      final provisioningPage = await homePage.openWifiProvisioningPage();

      provisioningFactory = () {
        mockProvisioning = MockProvisioning();
        when(mockProvisioning.establishSession()).thenAnswer(
          (_) => Future.delayed(const Duration(milliseconds: 500), () => true),
        );
        when(mockProvisioning.startScanWiFi()).thenAnswer(
          (_) => Future.value([
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
          ]),
        );
      };

      await provisioningPage.startProvisioning();

      await tester.pump(const Duration(seconds: 1));

      final ssidCard = find.textContaining('test SSID');
      expect(ssidCard, findsOneWidget);

      await tester.tap(ssidCard.first);
      await tester.pumpAndSettle();

      when(
        mockProvisioning.sendWifiConfig(
          ssid: 'test SSID',
          password: 'password',
        ),
      ).thenAnswer(
        (_) => Future.delayed(const Duration(seconds: 1), () => true),
      );
      when(
        mockProvisioning.applyWifiConfig(),
      ).thenAnswer((_) => Future.value(true));
      when(
        mockProvisioning.getStatus(),
      ).thenAnswer(
        (_) => Future.value(
          ConnectionStatus(
            state: WifiConnectionState.Connected,
            ip: '1.2.3.4',
          ),
        ),
      );

      await provisioningPage.enterPassword('password');
      await provisioningPage.submitPassword();

      await tester.pump(const Duration(milliseconds: 500));

      expect(find.text('Testing...'), findsOneWidget);

      await tester.pump(const Duration(seconds: 1));

      expect(provisioningPage.findSuccessPage, findsOneWidget);
    },
  );

  testWidgets('WiFi provisioning fails if not connected to access point',
      (tester) async {
    late MockProvisioning mockProvisioning;
    void Function()? provisioningFactory;
    MockProvisioning provisioningFactoryWrapper() {
      provisioningFactory!.call();
      return mockProvisioning;
    }

    final websocket = MockRobustWebsocket();

    await tester.pumpWidget(
      ProviderScope(
        overrides: [
          robustWebsocketProvider(kShrapnelUri).overrideWith((_) => websocket),
          provisioningProvider.overrideWith(
            (_) => WifiProvisioningService(
              provisioningFactory: provisioningFactoryWrapper,
            ),
          ),
        ],
        child: App(),
      ),
    );

    final homePage = HomePageObject(tester);
    final provisioningPage = await homePage.openWifiProvisioningPage();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession())
          .thenAnswer((_) => Future.value(false));
    };

    await provisioningPage.startProvisioning();

    expect(find.textContaining('failed'), findsOneWidget);
  });

  testWidgets('WiFi provisioning fails if incorrect password is provided',
      (tester) async {
    late MockProvisioning mockProvisioning;
    void Function()? provisioningFactory;
    MockProvisioning provisioningFactoryWrapper() {
      provisioningFactory!.call();
      return mockProvisioning;
    }

    final websocket = MockRobustWebsocket();

    await tester.pumpWidget(
      ProviderScope(
        overrides: [
          robustWebsocketProvider(kShrapnelUri).overrideWith((_) => websocket),
          provisioningProvider.overrideWith(
            (_) => WifiProvisioningService(
              provisioningFactory: provisioningFactoryWrapper,
            ),
          ),
        ],
        child: App(),
      ),
    );

    final homePage = HomePageObject(tester);
    final provisioningPage = await homePage.openWifiProvisioningPage();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession()).thenAnswer(
        (_) => Future.delayed(const Duration(milliseconds: 500), () => true),
      );
      when(mockProvisioning.startScanWiFi()).thenAnswer(
        (_) => Future.value([
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
        ]),
      );
    };

    await provisioningPage.startProvisioning();

    await tester.pump(const Duration(seconds: 1));

    final ssidCard = find.textContaining('test SSID');
    expect(ssidCard, findsOneWidget);

    await tester.tap(ssidCard.first);
    await tester.pumpAndSettle();

    when(
      mockProvisioning.sendWifiConfig(
        ssid: 'test SSID',
        password: 'password',
      ),
    ).thenAnswer((_) => Future.delayed(const Duration(seconds: 1), () => true));
    when(
      mockProvisioning.applyWifiConfig(),
    ).thenAnswer((_) => Future.value(true));
    when(
      mockProvisioning.getStatus(),
    ).thenAnswer(
      (_) => Future.value(
        ConnectionStatus(
          state: WifiConnectionState.ConnectionFailed,
          failedReason: WifiConnectFailedReason.AuthError,
        ),
      ),
    );

    await provisioningPage.enterPassword('password');
    await provisioningPage.submitPassword();
    verify(
      mockProvisioning.sendWifiConfig(
        ssid: 'test SSID',
        password: 'password',
      ),
    );

    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Testing...'), findsOneWidget);

    await tester.pump(const Duration(seconds: 1));

    expect(find.textContaining('failed to connect'), findsOneWidget);
  });

  testWidgets('WiFi provisioning fails if incorrect SSID is provided',
      (tester) async {
    late MockProvisioning mockProvisioning;
    void Function()? provisioningFactory;
    MockProvisioning provisioningFactoryWrapper() {
      provisioningFactory!.call();
      return mockProvisioning;
    }

    final websocket = MockRobustWebsocket();

    await tester.pumpWidget(
      ProviderScope(
        overrides: [
          robustWebsocketProvider(kShrapnelUri).overrideWith((_) => websocket),
          provisioningProvider.overrideWith(
            (_) => WifiProvisioningService(
              provisioningFactory: provisioningFactoryWrapper,
            ),
          ),
        ],
        child: App(),
      ),
    );

    final homePage = HomePageObject(tester);
    final provisioningPage = await homePage.openWifiProvisioningPage();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession()).thenAnswer(
        (_) => Future.delayed(const Duration(milliseconds: 500), () => true),
      );
      when(mockProvisioning.startScanWiFi()).thenAnswer(
        (_) => Future.value([
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
        ]),
      );
    };

    await provisioningPage.startProvisioning();

    await tester.pump(const Duration(seconds: 1));

    final ssidCard = find.textContaining('test SSID');
    expect(ssidCard, findsOneWidget);

    await tester.tap(ssidCard.first);
    await tester.pumpAndSettle();

    when(
      mockProvisioning.sendWifiConfig(
        ssid: 'test SSID',
        password: 'password',
      ),
    ).thenAnswer((_) => Future.delayed(const Duration(seconds: 1), () => true));
    when(
      mockProvisioning.applyWifiConfig(),
    ).thenAnswer((_) => Future.value(true));
    when(
      mockProvisioning.getStatus(),
    ).thenAnswer(
      (_) => Future.value(
        ConnectionStatus(
          state: WifiConnectionState.ConnectionFailed,
          failedReason: WifiConnectFailedReason.NetworkNotFound,
        ),
      ),
    );

    await provisioningPage.enterPassword('password');
    await provisioningPage.submitPassword();

    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Testing...'), findsOneWidget);

    await tester.pump(const Duration(seconds: 1));

    expect(find.textContaining('not found'), findsOneWidget);
  });

  testWidgets('WiFi provisioning fails if invalid password is provided',
      (tester) async {
    late MockProvisioning mockProvisioning;
    void Function()? provisioningFactory;
    MockProvisioning provisioningFactoryWrapper() {
      provisioningFactory!.call();
      return mockProvisioning;
    }

    final websocket = MockRobustWebsocket();

    await tester.pumpWidget(
      ProviderScope(
        overrides: [
          robustWebsocketProvider(kShrapnelUri).overrideWith((_) => websocket),
          provisioningProvider.overrideWith(
            (_) => WifiProvisioningService(
              provisioningFactory: provisioningFactoryWrapper,
            ),
          ),
        ],
        child: App(),
      ),
    );

    final homePage = HomePageObject(tester);
    final provisioningPage = await homePage.openWifiProvisioningPage();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession()).thenAnswer(
        (_) => Future.delayed(const Duration(milliseconds: 500), () => true),
      );
      when(mockProvisioning.startScanWiFi()).thenAnswer(
        (_) => Future.value([
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
        ]),
      );
    };

    await provisioningPage.startProvisioning();

    await tester.pump(const Duration(seconds: 1));

    final ssidCard = find.textContaining('test SSID');
    expect(ssidCard, findsOneWidget);

    await tester.tap(ssidCard.first);
    await tester.pumpAndSettle();

    Future<void> submitPassword(String password) async {
      await provisioningPage.enterPassword(password);
      await provisioningPage.submitPassword();
      await tester.pump();
    }

    await submitPassword('');
    expect(find.textContaining('must not be empty'), findsOneWidget);

    await submitPassword('a' * 7);
    expect(find.textContaining('longer than 7'), findsOneWidget);

    await submitPassword('a' * 65);
    expect(find.textContaining('shorter than 65'), findsOneWidget);

    await submitPassword('😊');
    expect(find.textContaining('contains invalid characters'), findsOneWidget);
  });

  testWidgets('WiFi provisioning can succeed when using hidden SSID',
      (tester) async {
    late MockProvisioning mockProvisioning;
    void Function()? provisioningFactory;
    MockProvisioning provisioningFactoryWrapper() {
      provisioningFactory!.call();
      return mockProvisioning;
    }

    final websocket = MockRobustWebsocket();

    await tester.pumpWidget(
      ProviderScope(
        overrides: [
          robustWebsocketProvider(kShrapnelUri).overrideWith((_) => websocket),
          provisioningProvider.overrideWith(
            (_) => WifiProvisioningService(
              provisioningFactory: provisioningFactoryWrapper,
            ),
          ),
        ],
        child: App(),
      ),
    );

    final homePage = HomePageObject(tester);
    final provisioningPage = await homePage.openWifiProvisioningPage();

    provisioningFactory = () {
      mockProvisioning = MockProvisioning();
      when(mockProvisioning.establishSession()).thenAnswer(
        (_) => Future.delayed(const Duration(milliseconds: 500), () => true),
      );
      when(mockProvisioning.startScanWiFi()).thenAnswer(
        (_) => Future.value([
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
        ]),
      );
    };

    await provisioningPage.startProvisioning();

    await tester.pump(const Duration(seconds: 1));

    final ssidCard = find.textContaining('test SSID');
    expect(ssidCard, findsOneWidget);

    await provisioningPage.openAdvancedSetup();

    when(
      mockProvisioning.sendWifiConfig(
        ssid: 'hidden SSID',
        password: 'password',
      ),
    ).thenAnswer((_) => Future.delayed(const Duration(seconds: 1), () => true));
    when(
      mockProvisioning.applyWifiConfig(),
    ).thenAnswer((_) => Future.value(true));
    when(
      mockProvisioning.getStatus(),
    ).thenAnswer(
      (_) => Future.value(
        ConnectionStatus(
          state: WifiConnectionState.Connected,
          ip: '1.2.3.4',
        ),
      ),
    );

    await provisioningPage.enterSsid('hidden SSID');

    await provisioningPage.enterPassword('password');
    await provisioningPage.submitAdvanced();

    await tester.pump(const Duration(milliseconds: 500));

    expect(find.text('Testing...'), findsOneWidget);

    await tester.pump(const Duration(seconds: 1));

    expect(provisioningPage.findSuccessPage, findsOneWidget);
  });
}
