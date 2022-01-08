import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'amplifier.dart';
import 'parameter.dart';

class _ValvestateParameterGain extends AudioParameterDouble {
  _ValvestateParameterGain() : super(name: "GAIN", id: "ampGain");
}

class _ValvestateParameterBass extends AudioParameterDouble {
  _ValvestateParameterBass() : super(name: "BASS", id: "bass");
}

class _ValvestateParameterMiddle extends AudioParameterDouble {
  _ValvestateParameterMiddle() : super(name: "MIDDLE", id: "middle");
}

class _ValvestateParameterTreble extends AudioParameterDouble {
  _ValvestateParameterTreble() : super(name: "TREBLE", id: "treble");
}

class _ValvestateParameterContour extends AudioParameterDouble {
  _ValvestateParameterContour() : super(name: "CONTOUR", id: "contour");
}

class _ValvestateParameterVolume extends AudioParameterDouble {
  _ValvestateParameterVolume() : super(name: "VOLUME", id: "volume");
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
              (value) {
                gain.value = value;
              },
              (value) {
                bass.value = value;
              },
              (value) {
                middle.value = value;
              },
              (value) {
                treble.value = value;
              },
              (value) {
                contour.value = value;
              },
              (value) {
                volume.value = value;
              },
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
        const ParameterUpdater<_ValvestateParameterGain, ParameterChannel>(),
        const ParameterUpdater<_ValvestateParameterBass, ParameterChannel>(),
        const ParameterUpdater<_ValvestateParameterMiddle, ParameterChannel>(),
        const ParameterUpdater<_ValvestateParameterTreble, ParameterChannel>(),
        const ParameterUpdater<_ValvestateParameterContour, ParameterChannel>(),
        const ParameterUpdater<_ValvestateParameterVolume, ParameterChannel>(),
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
  Widget build(BuildContext context) => MultiProvider(
        providers: [
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterGain(),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterBass(),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterMiddle(),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterTreble(),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterContour(),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterVolume(),
          ),
        ],
        child: child,
      );
}
