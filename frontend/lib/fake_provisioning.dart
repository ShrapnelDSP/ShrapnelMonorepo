import 'dart:math';
import 'dart:typed_data';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
// ignore: implementation_imports
import 'package:esp_softap_provisioning/src/connection_models.dart';

class FakeProvisioning extends ProvisioningBase {
  @override
  Future<bool> applyWifiConfig() =>
      Future.delayed(const Duration(milliseconds: 1000), () => true);

  @override
  Future<bool> establishSession() =>
      Future.delayed(const Duration(milliseconds: 500), () => true);

  @override
  Future<ConnectionStatus?> getStatus() => Future.delayed(
      const Duration(milliseconds: 500),
      () => ConnectionStatus(state: WifiConnectionState.Connecting));

  @override
  Future<Uint8List> sendReceiveCustomData(Uint8List data,
      {int packageSize = 256}) {
    // TODO: implement sendReceiveCustomData
    throw UnimplementedError();
  }

  @override
  Future<bool> sendWifiConfig({String? ssid, String? password}) =>
      Future.delayed(const Duration(milliseconds: 1000), () => true);

  @override
  Future<List<Map<String, dynamic>>?> startScanWiFi() => Future.delayed(
      const Duration(milliseconds: 500),
      () => <Map<String, dynamic>>[
            _createFakeWifi(0),
            _createFakeWifi(1),
            _createFakeWifi(2),
            _createFakeWifi(3),
          ]);

  Map<String, dynamic> _createFakeWifi(int id) => <String, dynamic>{
        'ssid': 'SSID $id',
        'channel': id,
        'rssi': -60 - 5 * id,
        'bssid': _createFakeMac(id),
        // TODO This is not a realistic value for the auth field. What are the
        //      actual possible values?
        'auth': 'wpa',
      };

  List<int> _createFakeMac(int id) {
    final rand = Random(id);
    final mac = <int>[];

    for (var i = 0; i < 6; i++) {
      mac.add(rand.nextInt(256));
    }

    return mac;
  }
}
