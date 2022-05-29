import 'dart:typed_data';

import 'package:esp_softap_provisioning/esp_softap_provisioning.dart';
// ignore: implementation_imports
import 'package:esp_softap_provisioning/src/connection_models.dart';

class FakeProvisioning extends ProvisioningBase {
  @override
  Future<bool> applyWifiConfig() {
    // TODO: implement applyWifiConfig
    throw UnimplementedError();
  }

  @override
  Future<bool> establishSession() =>
      Future.delayed(const Duration(seconds: 2), () => false);

  @override
  Future<ConnectionStatus?> getStatus() {
    // TODO: implement getStatus
    throw UnimplementedError();
  }

  @override
  Future<Uint8List> sendReceiveCustomData(Uint8List data,
      {int packageSize = 256}) {
    // TODO: implement sendReceiveCustomData
    throw UnimplementedError();
  }

  @override
  Future<bool> sendWifiConfig({String? ssid, String? password}) {
    // TODO: implement sendWifiConfig
    throw UnimplementedError();
  }

  @override
  Future<List<Map<String, dynamic>>?> startScanWiFi() => Future.delayed(
      const Duration(seconds: 2),
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
        'bssid': '01:23:45:67:89:ab',
        'auth': 'wpa',
      };
}
