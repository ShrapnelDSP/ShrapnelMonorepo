import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:stream_channel/stream_channel.dart';

import 'amplifier.dart';
import 'parameter.dart';

class ValvestateParameterGain extends AudioParameterDouble with ChangeNotifier {
  ValvestateParameterGain() : super(name: "GAIN", id: "ampGain");

  set gain(double gain) {
    value = gain;
    notifyListeners();
  }

  double get gain => value;
}

class ValvestateParameterBass extends AudioParameterDouble with ChangeNotifier {
  ValvestateParameterBass() : super(name: "BASS", id: "bass");

  set bass(double bass) {
    value = bass;
    notifyListeners();
  }

  double get bass => value;
}

class ValvestateParameterMiddle extends AudioParameterDouble
    with ChangeNotifier {
  ValvestateParameterMiddle() : super(name: "MIDDLE", id: "middle");

  set middle(double middle) {
    value = middle;
    notifyListeners();
  }

  double get middle => value;
}

class ValvestateParameterTreble extends AudioParameterDouble
    with ChangeNotifier {
  ValvestateParameterTreble() : super(name: "TREBLE", id: "treble");

  set treble(double treble) {
    value = treble;
    notifyListeners();
  }

  double get treble => value;
}

class ValvestateParameterContour extends AudioParameterDouble
    with ChangeNotifier {
  ValvestateParameterContour() : super(name: "CONTOUR", id: "contour");

  set contour(double contour) {
    value = contour;
    notifyListeners();
  }

  double get contour => value;
}

class ValvestateParameterVolume extends AudioParameterDouble
    with ChangeNotifier {
  ValvestateParameterVolume() : super(name: "VOLUME", id: "volume");

  set volume(double volume) {
    value = volume;
    notifyListeners();
  }

  double get volume => value;
}

/* TODO
 *
 * - How to group together all the parameters so they can be listened and
 *   provided at once?
 */

class Valvestate extends StatelessWidget {
  final StreamChannel channel;

  const Valvestate({
    Key? key,
    required this.channel,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Consumer6<
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
        ),
        ParameterUpdater<ValvestateParameterGain>(channel: channel),
        ParameterUpdater<ValvestateParameterBass>(channel: channel),
        ParameterUpdater<ValvestateParameterMiddle>(channel: channel),
        ParameterUpdater<ValvestateParameterTreble>(channel: channel),
        ParameterUpdater<ValvestateParameterContour>(channel: channel),
        ParameterUpdater<ValvestateParameterVolume>(channel: channel),
      ],
    );
  }
}
