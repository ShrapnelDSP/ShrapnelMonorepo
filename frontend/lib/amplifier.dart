import 'package:flutter/material.dart';
import 'knob.dart';

class Amplifier extends StatelessWidget {
  const Amplifier({
    Key? key,
    required this.parameter,
    required this.onChanged,
    required this.parameterName,
    required this.name,
    required this.onTap,
    required this.full,
  }) : super(key: key);

  final List<double> parameter;
  final List<ValueChanged<double>> onChanged;
  final List<String> parameterName;

  final bool full;
  final Function() onTap;

  final String name;

  Widget knobWithLabel(int index, double scaleFactor) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Knob(
          value: parameter[index],
          onChanged:
              full ? onChanged[index] : (ignored) {/* not interactive */},
          size: scaleFactor * 25,
        ),
        if (full) const SizedBox(height: 10),
        if (full)
          Text(
            parameterName[index],
            textAlign: TextAlign.center,
          ),
      ],
    );
  }

  List<Widget> knobs(double scaleFactor) {
    final knobs = <Widget>[];

    for (var i = 0; i < parameter.length; i++) {
      knobs.add(knobWithLabel(i, scaleFactor));
      if (i < parameter.length - 1) {
        knobs.add(SizedBox(width: scaleFactor * 10));
      }
    }

    return knobs;
  }

  @override
  Widget build(BuildContext context) {
    final scaleFactor = full ? 3.0 : 1.0;

    return GestureDetector(
      onTap: onTap,
      child: Card(
        child: Container(
          margin: EdgeInsets.all(scaleFactor * 10),
          child: Row(
            children: knobs(scaleFactor),
          ),
        ),
      ),
    );
  }
}
