import 'package:flutter/material.dart';
import 'knob.dart';

class Amplifier extends StatefulWidget {
  final List<double> parameter;
  final List<ValueChanged<double>> onChanged;
  final List<String> parameterName;

  final String name;

  const Amplifier({
    Key? key,
    required this.parameter,
    required this.onChanged,
    required this.parameterName,
    required this.name,
  }) : super(key: key);

  @override
  _AmplifierState createState() => _AmplifierState();
}

class _AmplifierState extends State<Amplifier> {
  bool _full = false;

  Widget knobWithLabel(int index, double scaleFactor) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Knob(
          value: widget.parameter[index],
          onChanged: _full
              ? widget.onChanged[index]
              : (ignored) {/* not interactive */},
          size: scaleFactor * 25,
        ),
        if (_full) const SizedBox(height: 10),
        if (_full)
          Text(
            widget.parameterName[index],
            textAlign: TextAlign.center,
          ),
      ],
    );
  }

  List<Widget> knobs(double scaleFactor) {
    List<Widget> knobs = [];

    for (var i = 0; i < widget.parameter.length; i++) {
      knobs.add(knobWithLabel(i, scaleFactor));
      if(i < widget.parameter.length - 1) knobs.add(SizedBox(width: scaleFactor * 10));
    }

    return knobs;
  }

  void _toggleSize() => setState(() => _full = !_full);

  @override
  Widget build(BuildContext context) {
    double scaleFactor = _full ? 3 : 1;

    return GestureDetector(
            onTap: _toggleSize,
            child: Card(
              child: Container(
                margin: EdgeInsets.all(scaleFactor * 10),
                child: Row(
                  children: knobs(scaleFactor),),
),
),
    );
  }
}
