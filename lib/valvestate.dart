import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:stream_channel/stream_channel.dart';

import 'amplifier.dart';
import 'parameter.dart';

class _ValvestateParameterGain extends AudioParameterDouble with ChangeNotifier {
  _ValvestateParameterGain() : super(name: "GAIN", id: "ampGain");

  set gain(double gain) {
    value = gain;
    notifyListeners();
  }

  double get gain => value;
}

class _ValvestateParameterBass extends AudioParameterDouble with ChangeNotifier {
  _ValvestateParameterBass() : super(name: "BASS", id: "bass");

  set bass(double bass) {
    value = bass;
    notifyListeners();
  }

  double get bass => value;
}

class _ValvestateParameterMiddle extends AudioParameterDouble
    with ChangeNotifier {
  _ValvestateParameterMiddle() : super(name: "MIDDLE", id: "middle");

  set middle(double middle) {
    value = middle;
    notifyListeners();
  }

  double get middle => value;
}

class _ValvestateParameterTreble extends AudioParameterDouble
    with ChangeNotifier {
  _ValvestateParameterTreble() : super(name: "TREBLE", id: "treble");

  set treble(double treble) {
    value = treble;
    notifyListeners();
  }

  double get treble => value;
}

class _ValvestateParameterContour extends AudioParameterDouble
    with ChangeNotifier {
  _ValvestateParameterContour() : super(name: "CONTOUR", id: "contour");

  set contour(double contour) {
    value = contour;
    notifyListeners();
  }

  double get contour => value;
}

class _ValvestateParameterVolume extends AudioParameterDouble
    with ChangeNotifier {
  _ValvestateParameterVolume() : super(name: "VOLUME", id: "volume");

  set volume(double volume) {
    value = volume;
    notifyListeners();
  }

  double get volume => value;
}

class Valvestate extends StatelessWidget {
  const Valvestate({
    Key? key,
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
              gain.gain,
              bass.bass,
              middle.middle,
              treble.treble,
              contour.contour,
              volume.volume,
            ],
            onChanged: [
              (value) {
                gain.gain = value;
              },
              (value) {
                bass.bass = value;
              },
              (value) {
                middle.middle = value;
              },
              (value) {
                treble.treble = value;
              },
              (value) {
                contour.contour = value;
              },
              (value) {
                volume.volume = value;
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
