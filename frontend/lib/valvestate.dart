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

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'amplifier.dart';
import 'parameter.dart';

class Valvestate extends StatelessWidget {
  const Valvestate({
    Key? key,
    required this.full,
    required this.onTap,
  }) : super(key: key);

  static const _name = 'VALVESTATE 8100';
  final bool full;
  final void Function() onTap;

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) {
        final parameterService =
            Provider.of<ParameterService>(context, listen: false);

        return AmplifierModel(
          parameters: [
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'OD1/OD2',
              id: 'ampChannel',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'GAIN',
              id: 'ampGain',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'BASS',
              id: 'bass',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'MIDDLE',
              id: 'middle',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'TREBLE',
              id: 'treble',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'CONTOUR',
              id: 'contour',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              groupName: _name,
              name: 'VOLUME',
              id: 'volume',
              parameterService: parameterService,
            ),
          ],
        );
      },
      child: Amplifier(
        name: _name,
        onTap: onTap,
        full: full,
      ),
    );
  }
}
