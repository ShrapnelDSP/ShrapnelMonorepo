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
    required this.bypass,
    required this.full,
    required this.onTap,
  }) : super(key: key);

  final bool full;
  final void Function() onTap;
  final bool bypass;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Consumer4<_NoiseGateParameterAttack, _NoiseGateParameterHysteresis,
            _NoiseGateParameterRelease, _NoiseGateParameterThreshold>(
          builder: (_, attack, hysteresis, release, threshold, __) => Stompbox(
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
            bypass: bypass,
            name: 'Noise Gate',
            onTap: onTap,
            full: full,
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
