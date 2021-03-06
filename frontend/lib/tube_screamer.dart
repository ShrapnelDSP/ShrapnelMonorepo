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

class _TubeScreamerParameterBypass extends AudioParameterDoubleModel {
  _TubeScreamerParameterBypass({required ParameterService parameterService})
      : super(
          name: 'Bypass',
          id: 'tubeScreamerBypass',
          parameterService: parameterService,
        );
}

class _TubeScreamerParameterDrive extends AudioParameterDoubleModel {
  _TubeScreamerParameterDrive({required ParameterService parameterService})
      : super(
          name: 'DRIVE',
          id: 'tubeScreamerDrive',
          parameterService: parameterService,
        );
}

class _TubeScreamerParameterLevel extends AudioParameterDoubleModel {
  _TubeScreamerParameterLevel({required ParameterService parameterService})
      : super(
          name: 'LEVEL',
          id: 'tubeScreamerLevel',
          parameterService: parameterService,
        );
}

class _TubeScreamerParameterTone extends AudioParameterDoubleModel {
  _TubeScreamerParameterTone({required ParameterService parameterService})
      : super(
          name: 'TONE',
          id: 'tubeScreamerTone',
          parameterService: parameterService,
        );
}

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
    return Row(
      children: [
        Consumer4<_TubeScreamerParameterBypass, _TubeScreamerParameterDrive,
            _TubeScreamerParameterLevel, _TubeScreamerParameterTone>(
          builder: (_, bypass, drive, level, tone, __) => Stompbox(
            value: [
              drive.value,
              tone.value,
              level.value,
            ],
            onChanged: [
              drive.onUserChanged,
              tone.onUserChanged,
              level.onUserChanged,
            ],
            parameterName: [
              drive.name,
              tone.name,
              level.name,
            ],
            name: 'Tube Screamer',
            onCardTap: onTap,
            full: full,
            onBypassTap: () =>
                bypass.onUserChanged((bypass.value > 0.5) ? 0 : 1),
            bypass: bypass.value > 0.5,
            primarySwatch: Colors.green,
          ),
        ),
      ],
    );
  }
}

class TubeScreamerParameterProvider extends StatelessWidget {
  const TubeScreamerParameterProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  final Widget child;

  @override
  Widget build(BuildContext context) =>
      Consumer<ParameterService>(builder: (_, parameterService, __) {
        return MultiProvider(
          providers: [
            ChangeNotifierProvider(
              create: (_) => _TubeScreamerParameterBypass(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _TubeScreamerParameterDrive(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _TubeScreamerParameterLevel(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _TubeScreamerParameterTone(
                parameterService: parameterService,
              ),
            ),
          ],
          child: child,
        );
      });
}
