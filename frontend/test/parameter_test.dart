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

import 'parameter_test.mocks.dart';

@GenerateMocks([ParameterService])
void main() {
  test('Convert to json', () {
    final parameterService = MockParameterService();

    final parameter = AudioParameterDoubleModel(
      groupName: 'TestGroup',
      name: 'TestName',
      id: 'test',
      parameterService: parameterService,
    );
    parameter.setValue(0);

    final expected = <String, dynamic>{
      'messageType': 'parameterUpdate',
      'value': 0.0,
      'id': 'test',
    };

    final actual = ParameterServiceOutputMessageParameterUpdate(
      parameter: AudioParameterDoubleData(
        value: parameter.value.value,
        id: parameter.id,
      ),
    ).toJson();

    expect(actual, expected);
  });

  test(
    'Notifies ParameterService onUserChanged',
    () async {
      final parameterService = MockParameterService();

      final parameter = AudioParameterDoubleModel(
        groupName: 'TestGroup',
        name: 'TestName',
        id: 'test',
        parameterService: parameterService,
      );
      parameter.onUserChanged(0);

      await pumpEventQueue();

      verify(
        parameterService.parameterUpdatedByUser(
          AudioParameterDoubleData(
            value: parameter.value.value,
            id: parameter.id,
          ),
        ),
      );
    },
  );

  test('Adds itself to parameter service', () {
    final parameterService = MockParameterService();

    final parameter = AudioParameterDoubleModel(
      groupName: 'TestGroup',
      name: 'TestName',
      id: 'test',
      parameterService: parameterService,
    );
    parameter.setValue(0);

    verify(parameterService.registerParameter(parameter));
  });
}
