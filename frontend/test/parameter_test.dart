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

import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:shrapnel/parameter.dart';
import 'package:shrapnel/parameters_meta.dart';

import 'parameter_test.mocks.dart';
import 'riverpod_util.dart';

@GenerateMocks([ParameterService])
void main() {
  test(
    'Notifies ParameterService onUserChanged',
    () async {
      final parameterService = MockParameterService();

      final container = createContainer(
        overrides: [
          allParametersProvider.overrideWithValue(
            {
              'test': const AudioParameterMetaData(
                groupName: 'TestGroup',
                name: 'TestName',
                id: 'test',
                min: 0,
                max: 1,
                defaultValue: 0.5,
              ),
            },
          ),
          parameterServiceProvider.overrideWith((_) => parameterService),
        ],
      );

      final parameter =
          container.read(audioParameterDoubleModelProvider('test').notifier);
      parameter.onUserChanged(0);

      await pumpEventQueue();

      verify(
        parameterService.parameterUpdatedByUser(
          AudioParameterDoubleData(
            value: 0,
            id: parameter.parameterId,
          ),
        ),
      );
    },
  );
}
