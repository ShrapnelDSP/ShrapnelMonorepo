import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'amplifier.dart';

class AudioParameterDouble {
  final String name;
  final String id;

  AudioParameterDouble({
    required this.name,
    required this.id,
  });
}

class ValvestateParameterGain extends ChangeNotifier {
  double _gain = 0.5;

  final String name = "GAIN";
  final String id = "ampGain";

  set gain(double gain) {
    _gain = gain;
    notifyListeners();
  }

  double get gain => _gain;
}

class ValvestateParameterBass extends ChangeNotifier {
  double _bass = 0.5;

  final String name = "BASS";
  final String id = "bass";

  set bass(double bass) {
    _bass = bass;
    notifyListeners();
  }

  double get bass => _bass;
}

class ValvestateParameterMiddle extends ChangeNotifier {
  double _middle = 0.5;

  final String name = "MIDDLE";
  final String id = "middle";

  set middle(double middle) {
    _middle = middle;
    notifyListeners();
  }

  double get middle => _middle;
}

class ValvestateParameterTreble extends ChangeNotifier {
  double _treble = 0.5;

  final String name = "TREBLE";
  final String id = "treble";

  set treble(double treble) {
    _treble = treble;
    notifyListeners();
  }

  double get treble => _treble;
}

class ValvestateParameterContour extends ChangeNotifier {
  double _contour = 0.5;

  final String name = "CONTOUR";
  final String id = "contour";

  set contour(double contour) {
    _contour = contour;
    notifyListeners();
  }

  double get contour => _contour;
}

class ValvestateParameterVolume extends ChangeNotifier {
  double _volume = 0.5;

  final String name = "VOLUME";
  final String id = "volume";

  set volume(double volume) {
    _volume = volume;
    notifyListeners();
  }

  double get volume => _volume;
}

/* TODO
 *
 * - How to group together all the parameters so they can be listened and
 *   provided at once?
 * - Use a common base class to indicate that name and id are required?
 */

class Valvestate extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Consumer6<
        ValvestateParameterGain,
        ValvestateParameterBass,
        ValvestateParameterMiddle,
        ValvestateParameterTreble,
        ValvestateParameterContour,
        ValvestateParameterVolume>(
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
    );
  }
}
