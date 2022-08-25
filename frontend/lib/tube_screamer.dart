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

import 'parameter.dart';
import 'stompbox.dart';

class TubeScreamer extends StatelessWidget {
  const TubeScreamer({
    Key? key,
    required this.full,
    required this.onTap,
  }) : super(key: key);

  final bool full;
  final void Function() onTap;

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) {
        final parameterService = Provider.of<ParameterService>(
          context,
          listen: false,
        );

        return StompboxModel(
          parameters: [
            AudioParameterDoubleModel(
              name: 'DRIVE',
              id: 'tubeScreamerDrive',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              name: 'TONE',
              id: 'tubeScreamerTone',
              parameterService: parameterService,
            ),
            AudioParameterDoubleModel(
              name: 'LEVEL',
              id: 'tubeScreamerLevel',
              parameterService: parameterService,
            ),
          ],
          bypass: AudioParameterDoubleModel(
            name: 'Bypass',
            id: 'tubeScreamerBypass',
            parameterService: parameterService,
          ),
        );
      },
      child: Stompbox(
        name: 'Tube Screamer',
        onCardTap: onTap,
        full: full,
        primarySwatch: Colors.green,
      ),
    );
  }
}
