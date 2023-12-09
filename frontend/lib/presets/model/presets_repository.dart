import 'dart:async';

import 'package:rxdart/rxdart.dart';

import 'presets.dart';
import 'presets_client.dart';
import 'presets_service.dart';

class PresetsRepository implements PresetsRepositoryBase {
  PresetsRepository({required this.client}) {
    client.connectionStream.listen((_) {
      unawaited(client.initialise());
    });
  }

  PresetsClient client;

  @override
  Future<PresetRecord> create(PresetState preset) async {
    await client.create(preset);

    // FIXME: find a more robust solution to linking up responses to requests.
    // possibly a lower layer protocol that includes request ID for every
    // request, that's also included in the responses
    final record = await client.presetUpdates
        .firstWhere((element) => element.preset == preset)
        .timeout(const Duration(seconds: 1));

    final newValue = _presets.value..[record.id] = record;
    _presets.add(newValue);

    return record;
  }

  @override
  Future<void> delete(int id) async {
    await client.delete(id);

    final newValue = _presets.value..remove(id);
    _presets.add(newValue);
  }

  final _presets = BehaviorSubject.seeded(<int, PresetRecord>{});

  @override
  ValueStream<Map<int, PresetRecord>> get presets => _presets;

  @override
  Future<void> update(PresetRecord preset) async {
    await client.update(preset);

    final newValue = _presets.value..[preset.id] = preset;
    _presets.add(newValue);
  }

  void dispose() {
    client.dispose();
    unawaited(_presets.close());
  }
}
