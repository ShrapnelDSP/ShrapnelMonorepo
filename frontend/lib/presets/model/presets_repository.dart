import 'dart:async';

import 'package:rxdart/rxdart.dart';
import 'package:uuid/uuid.dart';

import 'presets.dart';
import 'presets_client.dart';
import 'presets_service.dart';

class PresetsRepository implements PresetsRepositoryBase {
  PresetsRepository({required this.client}) {
    unawaited(client.initialise());
  }

  PresetsClient client;

  @override
  Future<void> create(PresetState preset) async {
    await client.create(preset);

    final newValue = _presets.value..[preset.id] = preset;
    _presets.add(newValue);
  }

  @override
  Future<void> delete(UuidValue id) async {
    await client.delete(id);

    final newValue = _presets.value..remove(id);
    _presets.add(newValue);
  }

  final _presets = BehaviorSubject.seeded(<UuidValue, PresetState>{});

  @override
  ValueStream<Map<UuidValue, PresetState>> get presets => _presets;

  @override
  Future<void> update(PresetState preset) async {
    await client.update(preset);

    final newValue = _presets.value..[preset.id] = preset;
    _presets.add(newValue);
  }

  void dispose() {
    client.dispose();
    unawaited(_presets.close());
  }
}
