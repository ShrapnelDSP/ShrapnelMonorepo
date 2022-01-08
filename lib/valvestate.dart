import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'amplifier.dart';
import 'parameter.dart';

class _ValvestateParameterGain extends AudioParameterDouble {
  _ValvestateParameterGain({required ParameterChannel parameterChannel})
      : super(
          name: "GAIN",
          id: "ampGain",
          parameterChannel: parameterChannel,
        );
}

class _ValvestateParameterBass extends AudioParameterDouble {
  _ValvestateParameterBass({required ParameterChannel parameterChannel})
      : super(
          name: "BASS",
          id: "bass",
          parameterChannel: parameterChannel,
        );
}

class _ValvestateParameterMiddle extends AudioParameterDouble {
  _ValvestateParameterMiddle({required ParameterChannel parameterChannel})
      : super(
          name: "MIDDLE",
          id: "middle",
          parameterChannel: parameterChannel,
        );
}

class _ValvestateParameterTreble extends AudioParameterDouble {
  _ValvestateParameterTreble({required ParameterChannel parameterChannel})
      : super(
          name: "TREBLE",
          id: "treble",
          parameterChannel: parameterChannel,
        );
}

class _ValvestateParameterContour extends AudioParameterDouble {
  _ValvestateParameterContour({required ParameterChannel parameterChannel})
      : super(
          name: "CONTOUR",
          id: "contour",
          parameterChannel: parameterChannel,
        );
}

class _ValvestateParameterVolume extends AudioParameterDouble {
  _ValvestateParameterVolume({required ParameterChannel parameterChannel})
      : super(
          name: "VOLUME",
          id: "volume",
          parameterChannel: parameterChannel,
        );
}

class Valvestate extends StatelessWidget {
  final bool full;
  final void Function() onTap;

  const Valvestate({
    Key? key,
    required this.full,
    required this.onTap,
  }) : super(key: key);

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
            name: "VALVESTATE 8100",
            onTap: onTap,
            full: full,
          ),
        ),
      ],
    );
  }
}

class ValvestateParameterProvider extends StatelessWidget {
  final Widget child;

  const ValvestateParameterProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) =>
      Consumer<ParameterChannel>(builder: (_, parameterChannel, __) {
        return MultiProvider(
          providers: [
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterGain(
                parameterChannel: parameterChannel,
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
