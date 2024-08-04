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

import 'package:logging/logging.dart';
import 'package:rxdart/rxdart.dart';
import 'package:shrapnel_common/api_data/api_message.dart';
import 'package:shrapnel_common/api_data/presets_message.dart';

import '../../api/api_websocket.dart';
import '../../core/message_transport.dart';
import '../../core/stream_extensions.dart';
import 'presets.dart' as presets;

// ignore: unused_element
final _log = Logger('presets_client');

extension PresetParametersDataEx on PresetParametersData {
  static PresetParametersData fromData(
    presets.PresetParametersData parameters,
  ) {
    return PresetParametersData(
      ampChannel: parameters.ampChannel,
      ampGain: parameters.ampGain,
      bass: parameters.bass,
      chorusBypass: parameters.chorusBypass,
      chorusDepth: parameters.chorusDepth,
      chorusMix: parameters.chorusMix,
      chorusRate: parameters.chorusRate,
      contour: parameters.contour,
      middle: parameters.middle,
      noiseGateAttack: parameters.noiseGateAttack,
      noiseGateBypass: parameters.noiseGateBypass,
      noiseGateHold: parameters.noiseGateHold,
      noiseGateHysteresis: parameters.noiseGateHysteresis,
      noiseGateRelease: parameters.noiseGateRelease,
      noiseGateThreshold: parameters.noiseGateThreshold,
      treble: parameters.treble,
      volume: parameters.volume,
      wahBypass: parameters.wahBypass,
      wahPosition: parameters.wahPosition,
      wahVocal: parameters.wahVocal,
    );
  }
}

extension PresetDataEx on PresetData {
  static PresetData fromPresetState(presets.PresetState presetState) {
    return PresetData(
      name: presetState.name,
      parameters: PresetParametersDataEx.fromData(presetState.parameters),
    );
  }
}

class PresetsTransport
    implements ReconnectingMessageTransport<PresetsMessage, PresetsMessage> {
  PresetsTransport({required this.websocket}) {
    _controller.stream
        .logFinest(_log, (event) => 'send message: $event')
        .map((message) => ApiMessage.presets(message: message))
        .listen(websocket.sink.add);
  }

  final _controller = StreamController<PresetsMessage>();

  @override
  StreamSink<PresetsMessage> get sink => _controller.sink;

  @override
  late final Stream<PresetsMessage> stream = websocket.stream
      .whereType<ApiMessagePresets>()
      .map((event) => event.message)
      .logFinest(
        _log,
        (event) => 'receive message: $event',
      );

  @override
  Stream<void> get connectionStream => websocket.connectionStream;

  ApiWebsocket websocket;

  @override
  void dispose() {
    unawaited(_controller.close());
  }

  @override
  bool get isAlive => websocket.isAlive;
}

class PresetsClient {
  PresetsClient({
    required ReconnectingMessageTransport<PresetsMessage, PresetsMessage>
        transport,
  }) : _transport = transport;

  final ReconnectingMessageTransport<PresetsMessage, PresetsMessage> _transport;

  /// Firmware will reply by sending a preset update message for every preset
  /// that already exists.
  Future<void> initialise() async {
    _transport.sink.add(PresetsMessage.initialise());
  }

  late final Stream<presets.PresetRecord> presetUpdates = _transport.stream
      .whereType<PresetsMessageNotify>()
      .map(
        (event) => presets.PresetRecord(
          id: event.id,
          preset: presets.PresetState(
            name: event.preset.name,
            parameters: presets.PresetParametersData(
              ampChannel: event.preset.parameters.ampChannel,
              ampGain: event.preset.parameters.ampGain,
              bass: event.preset.parameters.bass,
              chorusBypass: event.preset.parameters.chorusBypass,
              chorusDepth: event.preset.parameters.chorusDepth,
              chorusMix: event.preset.parameters.chorusMix,
              chorusRate: event.preset.parameters.chorusRate,
              contour: event.preset.parameters.contour,
              middle: event.preset.parameters.middle,
              noiseGateAttack: event.preset.parameters.noiseGateAttack,
              noiseGateBypass: event.preset.parameters.noiseGateBypass,
              noiseGateHold: event.preset.parameters.noiseGateHold,
              noiseGateHysteresis: event.preset.parameters.noiseGateHysteresis,
              noiseGateRelease: event.preset.parameters.noiseGateRelease,
              noiseGateThreshold: event.preset.parameters.noiseGateThreshold,
              treble: event.preset.parameters.treble,
              volume: event.preset.parameters.volume,
              wahBypass: event.preset.parameters.wahBypass,
              wahPosition: event.preset.parameters.wahPosition,
              wahVocal: event.preset.parameters.wahVocal,
            ),
          ),
        ),
      )
      .asBroadcastStream();

  Future<void> create(presets.PresetState preset) async {
    _transport.sink
        .add(PresetsMessage.create(PresetDataEx.fromPresetState(preset)));
  }

  Future<void> delete(int id) async {
    _transport.sink.add(PresetsMessage.delete(id));
  }

  Future<void> update(presets.PresetRecord preset) async {
    _transport.sink.add(
      PresetsMessage.update(
        preset.id,
        PresetDataEx.fromPresetState(preset.preset),
      ),
    );
  }

  Stream<void> get connectionStream => _transport.connectionStream;

  void dispose() {
    _transport.dispose();
  }
}
