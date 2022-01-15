import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'parameter.dart';
import 'stompbox.dart';

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
        Consumer3<_ChorusParameterDepth, _ChorusParameterMix,
            _ChorusParameterRate>(
          builder: (_, depth, mix, rate, __) => Stompbox(
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
            bypass: bypass,
            name: 'Chorus',
            onTap: onTap,
            full: full,
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
