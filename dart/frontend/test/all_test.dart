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

// ignore_for_file: unused_import

/* This file includes all other files in the project, so that untested files
 * show up propely in test coverage.
 */

import 'package:shrapnel/amplifier.dart';
import 'package:shrapnel/api/api_websocket.dart';
import 'package:shrapnel/audio_events.dart';
import 'package:shrapnel/chorus.dart';
import 'package:shrapnel/heavy_metal.dart';
import 'package:shrapnel/knob.dart';
import 'package:shrapnel/knob_with_label.dart';
import 'package:shrapnel/main.dart';
import 'package:shrapnel/midi_mapping/model/midi_learn.dart';
import 'package:shrapnel/midi_mapping/model/midi_learn_state.dart';
import 'package:shrapnel/midi_mapping/model/service.dart';
import 'package:shrapnel/midi_mapping/view/midi_learn.dart';
import 'package:shrapnel/midi_mapping/view/midi_mapping.dart';
import 'package:shrapnel/noise_gate.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/pedalboard.dart';
import 'package:shrapnel/robust_websocket.dart';
import 'package:shrapnel/status/view/websocket_status.dart';
import 'package:shrapnel/stompbox.dart';
import 'package:shrapnel/tube_screamer.dart';
import 'package:shrapnel/util/conditional_parent.dart';
import 'package:shrapnel/valvestate.dart';
import 'package:shrapnel/wah.dart';
import 'package:shrapnel/wifi_provisioning.dart';

void main() {}
