import 'package:flutter/material.dart';
import 'knob.dart';

class Stompbox extends StatelessWidget {
  final List<double> value;
  final List<ValueChanged<double>> onChanged;
  final List<String> parameterName;

  final bool bypass;
  final String name;

  final bool full;
  final Function() onTap;

  final MaterialColor primarySwatch;

  const Stompbox({
    Key? key,
    required this.value,
    required this.onChanged,
    required this.parameterName,
    required this.bypass,
    required this.name,
    required this.full,
    required this.onTap,
    required this.primarySwatch,
  }) : super(key: key);

  Widget knobWithLabel(int index, double scaleFactor) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Knob(
          value: value[index],
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
    if (value.length == 1) {
      return [
        Positioned(
          top: 0,
          child: knobWithLabel(0, scaleFactor),
        ),
      ];
    } else if (value.length == 2) {
      return [
        Positioned(
          left: 0,
          top: 0,
          child: knobWithLabel(0, scaleFactor),
        ),
        Positioned(
          right: 0,
          top: 0,
          child: knobWithLabel(1, scaleFactor),
        ),
      ];
    } else if (value.length == 3) {
      return [
        Positioned(
          left: 0,
          top: 0,
          child: knobWithLabel(0, scaleFactor),
        ),
        Positioned(
          right: 0,
          top: 0,
          child: knobWithLabel(1, scaleFactor),
        ),
        Positioned(
          top: scaleFactor * 35,
          child: knobWithLabel(2, scaleFactor),
        ),
      ];
    } else if (value.length == 4) {
      return [
        Positioned(
          left: 0,
          top: 0,
          child: knobWithLabel(0, scaleFactor),
        ),
        Positioned(
          right: 0,
          top: 0,
          child: knobWithLabel(1, scaleFactor),
        ),
        Positioned(
          left: 0,
          top: scaleFactor * 35,
          child: knobWithLabel(2, scaleFactor),
        ),
        Positioned(
          right: 0,
          top: scaleFactor * 35,
          child: knobWithLabel(3, scaleFactor),
        ),
      ];
    }

    assert(false, "Number of parameters must be between 1 and 4");
    return [];
  }

  @override
  Widget build(BuildContext context) {
    double scaleFactor = full ? 3 : 1;

    return Theme(
      data: ThemeData(
        brightness: Theme.of(context).brightness,
        primarySwatch: primarySwatch,
      ),
      /* Builder required to create new context, which makes
       * Theme.of return the new theme defined above
       */
      child: Builder(
        builder: (context) => SizedBox(
          width: scaleFactor * 100,
          height: scaleFactor * 150,
          child: GestureDetector(
            onTap: onTap,
            child: Card(
              child: Container(
                margin: EdgeInsets.all(scaleFactor * 10),
                child: Stack(alignment: Alignment.center, children: <Widget>[
                  ...knobs(scaleFactor),
                  Positioned(
                    top: scaleFactor * 70,
                    child: Text(name),
                  ),
                  Positioned(
                    top: scaleFactor * 95,
                    child: Container(
                      width: scaleFactor * 25,
                      height: scaleFactor * 25,
                      decoration: BoxDecoration(
                        color: Theme.of(context).colorScheme.background,
                        shape: BoxShape.circle,
                      ),
                    ),
                  ),
                  Positioned(
                    top: scaleFactor * 98,
                    child: Container(
                      width: scaleFactor * 19,
                      height: scaleFactor * 19,
                      decoration: BoxDecoration(
                        color: bypass
                            ? Theme.of(context).colorScheme.surface
                            : Theme.of(context).colorScheme.primary,
                        shape: BoxShape.circle,
                      ),
                    ),
                  ),
                ]),
              ),
            ),
          ),
        ),
      ),
    );
  }
}
