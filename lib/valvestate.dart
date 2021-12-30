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

/* TODO
 *
 * - How to group together all the parameters so they can be listened and
 *   provided at once?
 * - Use a common base class to indicate that name and id are required?
 */
class ValvestateModel extends ChangeNotifier {
  final bass = AudioParameterDouble(
    name: "BASS",
    id: "bass",
  );

  final middle = AudioParameterDouble(
    name: "MIDDLE",
    id: "middle",
  );

  final treble = AudioParameterDouble(
    name: "TREBLE",
    id: "treble",
  );

  final contour = AudioParameterDouble(
    name: "CONTOUR",
    id: "contour",
  );

  final volume = AudioParameterDouble(
    name: "VOLUME",
    id: "volume",
  );

  /*
  ValvestateModel({
      return ChangeNotifierProxyProvider<ValvestateParameterGain, ValvestateModel>(
            create: (_) => MyChangeNotifier(),
  update: (_, myModel, myNotifier) => myNotifier
    ..update(myModel),
  child: ...
);
  });
  */
}

class Valvestate extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Consumer<ValvestateParameterGain>(
      builder: (context, model, _) => Amplifier(
        parameter: [
          model.gain,
          model.gain,
          model.gain,
          model.gain,
          model.gain,
          model.gain,
        ],
        onChanged: [
          (value) {
            model.gain = value;
          },
          (value) {
            model.gain = value;
          },
          (value) {
            model.gain = value;
          },
          (value) {
            model.gain = value;
          },
          (value) {
            model.gain = value;
          },
          (value) {
            model.gain = value;
          },
        ],
        parameterName: [
          model.name,
          model.name,
          model.name,
          model.name,
          model.name,
          model.name,
        ],
        name: "VALVESTATE 8100",
      ),
    );
  }
}
