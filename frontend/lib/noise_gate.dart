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

class _NoiseGateParameterAttack extends AudioParameterDoubleModel {
  _NoiseGateParameterAttack({required ParameterService parameterService})
      : super(
          name: 'Attack',
          id: 'noiseGateAttack',
          parameterService: parameterService,
        );
}

class _NoiseGateParameterBypass extends AudioParameterDoubleModel {
  _NoiseGateParameterBypass({required ParameterService parameterService})
      : super(
          name: 'Bypass',
          id: 'noiseGateBypass',
          parameterService: parameterService,
        );
}

class _NoiseGateParameterHysteresis extends AudioParameterDoubleModel {
  _NoiseGateParameterHysteresis({required ParameterService parameterService})
      : super(
          name: 'Hysteresis',
          id: 'noiseGateHysteresis',
          parameterService: parameterService,
        );
}

class _NoiseGateParameterRelease extends AudioParameterDoubleModel {
  _NoiseGateParameterRelease({required ParameterService parameterService})
      : super(
          name: 'Release',
          id: 'noiseGateRelease',
          parameterService: parameterService,
        );
}

class _NoiseGateParameterThreshold extends AudioParameterDoubleModel {
  _NoiseGateParameterThreshold({required ParameterService parameterService})
      : super(
          name: 'Threshold',
          id: 'noiseGateThreshold',
          parameterService: parameterService,
        );
}

class NoiseGate extends StatelessWidget {
  const NoiseGate({
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
            _NoiseGateParameterAttack,
            _NoiseGateParameterBypass,
            _NoiseGateParameterHysteresis,
            _NoiseGateParameterRelease,
            _NoiseGateParameterThreshold>(
          builder: (_, attack, bypass, hysteresis, release, threshold, __) =>
              Stompbox(
            value: [
              threshold.value,
              hysteresis.value,
              attack.value,
              release.value,
            ],
            onChanged: [
              threshold.onUserChanged,
              hysteresis.onUserChanged,
              attack.onUserChanged,
              release.onUserChanged,
            ],
            parameterName: [
              threshold.name,
              hysteresis.name,
              attack.name,
              release.name,
            ],
            name: 'Noise Gate',
            onCardTap: onTap,
            full: full,
            onBypassTap: () =>
                bypass.onUserChanged((bypass.value > 0.5) ? 0 : 1),
            bypass: bypass.value > 0.5,
            primarySwatch: Colors.red,
          ),
        ),
      ],
    );
  }
}

class NoiseGateParameterProvider extends StatelessWidget {
  const NoiseGateParameterProvider({
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
              create: (_) => _NoiseGateParameterAttack(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _NoiseGateParameterBypass(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _NoiseGateParameterHysteresis(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _NoiseGateParameterRelease(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _NoiseGateParameterThreshold(
                parameterService: parameterService,
              ),
            ),
          ],
          child: child,
        );
      });
}
