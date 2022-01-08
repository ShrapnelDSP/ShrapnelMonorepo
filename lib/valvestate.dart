import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'amplifier.dart';
import 'parameter.dart';

class _ValvestateParameterGain extends AudioParameterDouble {
  _ValvestateParameterGain(ParameterChannel backend)
      : super(
          name: "GAIN",
          id: "ampGain",
          backendChannel: backend,
        );
}

class _ValvestateParameterBass extends AudioParameterDouble {
  _ValvestateParameterBass(ParameterChannel backend)
      : super(
          name: "BASS",
          id: "bass",
          backendChannel: backend,
        );
}

class _ValvestateParameterMiddle extends AudioParameterDouble {
  _ValvestateParameterMiddle(ParameterChannel backend)
      : super(
          name: "MIDDLE",
          id: "middle",
          backendChannel: backend,
        );
}

class _ValvestateParameterTreble extends AudioParameterDouble {
  _ValvestateParameterTreble(ParameterChannel backend)
      : super(
          name: "TREBLE",
          id: "treble",
          backendChannel: backend,
        );
}

class _ValvestateParameterContour extends AudioParameterDouble {
  _ValvestateParameterContour(ParameterChannel backend)
      : super(
          name: "CONTOUR",
          id: "contour",
          backendChannel: backend,
        );
}

class _ValvestateParameterVolume extends AudioParameterDouble {
  _ValvestateParameterVolume(ParameterChannel backend)
      : super(
          name: "VOLUME",
          id: "volume",
          backendChannel: backend,
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
  Widget build(BuildContext context) => MultiProvider(
        providers: [
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterGain(Provider.of<ParameterChannel>(context)),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterBass(Provider.of<ParameterChannel>(context)),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterMiddle(Provider.of<ParameterChannel>(context)),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterTreble(Provider.of<ParameterChannel>(context)),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterContour(Provider.of<ParameterChannel>(context)),
          ),
          ChangeNotifierProvider(
            create: (_) => _ValvestateParameterVolume(Provider.of<ParameterChannel>(context)),
          ),
        ],
        child: child,
      );
}
