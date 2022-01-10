import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'amplifier.dart';
import 'parameter.dart';

class _ValvestateParameterGain extends AudioParameterDoubleModel {
  _ValvestateParameterGain({required ParameterService parameterService})
      : super(
          name: 'GAIN',
          id: 'ampGain',
          parameterService: parameterService,
        );
}

class _ValvestateParameterBass extends AudioParameterDoubleModel {
  _ValvestateParameterBass({required ParameterService parameterChannel})
      : super(
          name: 'BASS',
          id: 'bass',
          parameterService: parameterChannel,
        );
}

class _ValvestateParameterMiddle extends AudioParameterDoubleModel {
  _ValvestateParameterMiddle({required ParameterService parameterChannel})
      : super(
          name: 'MIDDLE',
          id: 'middle',
          parameterService: parameterChannel,
        );
}

class _ValvestateParameterTreble extends AudioParameterDoubleModel {
  _ValvestateParameterTreble({required ParameterService parameterChannel})
      : super(
          name: 'TREBLE',
          id: 'treble',
          parameterService: parameterChannel,
        );
}

class _ValvestateParameterContour extends AudioParameterDoubleModel {
  _ValvestateParameterContour({required ParameterService parameterChannel})
      : super(
          name: 'CONTOUR',
          id: 'contour',
          parameterService: parameterChannel,
        );
}

class _ValvestateParameterVolume extends AudioParameterDoubleModel {
  _ValvestateParameterVolume({required ParameterService parameterChannel})
      : super(
          name: 'VOLUME',
          id: 'volume',
          parameterService: parameterChannel,
        );
}

class Valvestate extends StatelessWidget {
  const Valvestate({
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
        Consumer6<
            _ValvestateParameterGain,
            _ValvestateParameterBass,
            _ValvestateParameterMiddle,
            _ValvestateParameterTreble,
            _ValvestateParameterContour,
            _ValvestateParameterVolume>(
          builder: (context, gain, bass, middle, treble, contour, volume, _) =>
              Amplifier(
            parameter: [
              gain.value,
              bass.value,
              middle.value,
              treble.value,
              contour.value,
              volume.value,
            ],
            onChanged: [
              gain.onUserChanged,
              bass.onUserChanged,
              middle.onUserChanged,
              treble.onUserChanged,
              contour.onUserChanged,
              volume.onUserChanged,
            ],
            parameterName: [
              gain.name,
              bass.name,
              middle.name,
              treble.name,
              contour.name,
              volume.name,
            ],
            name: 'VALVESTATE 8100',
            onTap: onTap,
            full: full,
          ),
        ),
      ],
    );
  }
}

class ValvestateParameterProvider extends StatelessWidget {
  const ValvestateParameterProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  final Widget child;

  @override
  Widget build(BuildContext context) =>
      Consumer<ParameterService>(builder: (_, parameterChannel, __) {
        return MultiProvider(
          providers: [
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterGain(
                parameterService: parameterChannel,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterBass(
                parameterChannel: parameterChannel,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterMiddle(
                parameterChannel: parameterChannel,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterTreble(
                parameterChannel: parameterChannel,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterContour(
                parameterChannel: parameterChannel,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterVolume(
                parameterChannel: parameterChannel,
              ),
            ),
          ],
          child: child,
        );
      });
}
