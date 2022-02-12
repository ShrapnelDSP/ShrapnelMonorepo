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

class _ChorusParameterBypass extends AudioParameterDoubleModel {
  _ChorusParameterBypass({required ParameterService parameterService})
      : super(
          name: 'Bypass',
          id: 'chorusBypass',
          parameterService: parameterService,
        );
}

class _ChorusParameterDepth extends AudioParameterDoubleModel {
  _ChorusParameterDepth({required ParameterService parameterService})
      : super(
          name: 'DEPTH',
          id: 'chorusDepth',
          parameterService: parameterService,
        );
}

class _ChorusParameterMix extends AudioParameterDoubleModel {
  _ChorusParameterMix({required ParameterService parameterService})
      : super(
          name: 'MIX',
          id: 'chorusMix',
          parameterService: parameterService,
        );
}

class _ChorusParameterRate extends AudioParameterDoubleModel {
  _ChorusParameterRate({required ParameterService parameterService})
      : super(
          name: 'RATE',
          id: 'chorusRate',
          parameterService: parameterService,
        );
}

class Chorus extends StatelessWidget {
  const Chorus({
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
        Consumer4<_ChorusParameterBypass, _ChorusParameterDepth,
            _ChorusParameterMix, _ChorusParameterRate>(
          builder: (_, bypass, depth, mix, rate, __) => Stompbox(
            value: [
              rate.value,
              depth.value,
              mix.value,
            ],
            onChanged: [
              rate.onUserChanged,
              depth.onUserChanged,
              mix.onUserChanged,
            ],
            parameterName: [
              rate.name,
              depth.name,
              mix.name,
            ],
            name: 'Chorus',
            onCardTap: onTap,
            full: full,
            onBypassTap: () =>
                bypass.onUserChanged((bypass.value > 0.5) ? 0 : 1),
            bypass: bypass.value > 0.5,
            primarySwatch: Colors.blue,
          ),
        ),
      ],
    );
  }
}

class ChorusParameterProvider extends StatelessWidget {
  const ChorusParameterProvider({
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
              create: (_) => _ChorusParameterBypass(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ChorusParameterDepth(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ChorusParameterMix(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ChorusParameterRate(
                parameterService: parameterService,
              ),
            ),
          ],
          child: child,
        );
      });
}
