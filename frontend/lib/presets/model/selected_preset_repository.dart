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

import 'dart:async';

import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:rxdart/rxdart.dart';

import 'presets_service.dart';
import 'selected_preset_client.dart';

final selectedPresetRepositoryProvider = Provider<SelectedPresetRepositoryBase>(
  (ref) =>
      SelectedPresetRepository(client: ref.read(selectedPresetClientProvider)),
);

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
    return client.selectPreset(presetId);
  }

  @override
  ValueStream<int> get selectedPreset => _subject;
}
