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

import 'package:logging/logging.dart';
import 'package:shrapnel_test_lib/shrapnel_test_lib.dart';

final _log = Logger('shrapnel.test_tool');

void main(List<String> arguments) {
  hierarchicalLoggingEnabled = true;
  Logger.root.onRecord.listen(print);

  _log.info('hello $arguments');
  nvsSave('nvs_${DateTime.now().toIso8601String()}.bin', port: '/dev/ttyUSB0');

  // TODO Add command to reload from file
}
