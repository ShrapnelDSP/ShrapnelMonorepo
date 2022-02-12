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

class _HeavyMetalParameterBypass extends AudioParameterDoubleModel {
  _HeavyMetalParameterBypass({required ParameterService parameterService})
      : super(
          name: 'Bypass',
          id: 'heavyMetalBypass',
          parameterService: parameterService,
        );
}

class _HeavyMetalParameterDistortion extends AudioParameterDoubleModel {
  _HeavyMetalParameterDistortion({required ParameterService parameterService})
      : super(
          name: 'DISTORTION',
          id: 'heavyMetalDistortion',
          parameterService: parameterService,
        );
}

class _HeavyMetalParameterHigh extends AudioParameterDoubleModel {
  _HeavyMetalParameterHigh({required ParameterService parameterService})
      : super(
          name: 'HIGH',
          id: 'heavyMetalHigh',
          parameterService: parameterService,
        );
}

class _HeavyMetalParameterLevel extends AudioParameterDoubleModel {
  _HeavyMetalParameterLevel({required ParameterService parameterService})
      : super(
          name: 'LEVEL',
          id: 'heavyMetalLevel',
          parameterService: parameterService,
        );
}

class _HeavyMetalParameterLow extends AudioParameterDoubleModel {
  _HeavyMetalParameterLow({required ParameterService parameterService})
      : super(
          name: 'LOW',
          id: 'heavyMetalLow',
          parameterService: parameterService,
        );
}

class HeavyMetal extends StatelessWidget {
  const HeavyMetal({
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
        Consumer5<
            _HeavyMetalParameterBypass,
            _HeavyMetalParameterDistortion,
            _HeavyMetalParameterHigh,
            _HeavyMetalParameterLevel,
            _HeavyMetalParameterLow>(
          builder: (_, bypass, distortion, high, level, low, __) => Stompbox(
            value: [
              level.value,
              low.value,
              high.value,
              distortion.value,
            ],
            onChanged: [
              level.onUserChanged,
              low.onUserChanged,
              high.onUserChanged,
              distortion.onUserChanged,
            ],
            parameterName: [
              level.name,
              low.name,
              high.name,
              distortion.name,
            ],
            name: 'Heavy Metal',
            onCardTap: onTap,
            full: full,
            onBypassTap: () =>
                bypass.onUserChanged((bypass.value > 0.5) ? 0 : 1),
            bypass: bypass.value > 0.5,
            primarySwatch: Colors.deepOrange,
          ),
        ),
      ],
    );
  }
}

class HeavyMetalParameterProvider extends StatelessWidget {
  const HeavyMetalParameterProvider({
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
              create: (_) => _HeavyMetalParameterBypass(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _HeavyMetalParameterDistortion(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _HeavyMetalParameterHigh(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _HeavyMetalParameterLevel(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _HeavyMetalParameterLow(
                parameterService: parameterService,
              ),
            ),
          ],
          child: child,
        );
      });
}
