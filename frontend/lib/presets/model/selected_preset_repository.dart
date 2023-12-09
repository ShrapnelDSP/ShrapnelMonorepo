import 'dart:async';

import 'package:rxdart/rxdart.dart';

import 'presets_service.dart';
import 'selected_preset_client.dart';

class SelectedPresetRepository implements SelectedPresetRepositoryBase {
  SelectedPresetRepository({required this.client}) {
    unawaited(_subject.addStream(client.selectedPreset));
    client.connectionStream.listen((_) {
      unawaited(client.initialise());
    });
  }

  SelectedPresetClient client;
  final _subject = BehaviorSubject<int>();

  @override
  Future<void> selectPreset(int presetId) async {
    _subject.value = presetId;
    return client.selectPreset(presetId);
  }

  @override
  ValueStream<int> get selectedPreset => _subject;
}
