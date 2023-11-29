import 'dart:async';

import 'package:rxdart/rxdart.dart';
import 'package:uuid/uuid.dart';

import 'presets_service.dart';
import 'selected_preset_client.dart';

class SelectedPresetRepository implements SelectedPresetRepositoryBase {
  SelectedPresetRepository({required this.client}) {
    unawaited(_subject.addStream(client.selectedPreset));
    // FIXME: initialise needs to run whenever we become connected, not in the
    // constructor. It could stay in the constructor as long as a new instance
    // of this is created every time we reconnect.
    client.initialise();
  }

  SelectedPresetClient client;
  final _subject = BehaviorSubject<UuidValue>();

  @override
  Future<void> selectPreset(UuidValue presetId) async {
    _subject.value = presetId;
    return client.selectPreset(presetId);
  }

  @override
  ValueStream<UuidValue> get selectedPreset => _subject;
}
