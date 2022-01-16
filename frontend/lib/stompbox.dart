import 'package:flutter/material.dart';
import 'knob.dart';

class Stompbox extends StatelessWidget {
  const Stompbox({
    Key? key,
    required this.value,
    required this.onChanged,
    required this.parameterName,
    required this.bypass,
    required this.name,
    required this.full,
    required this.onCardTap,
    required this.onBypassTap,
    required this.primarySwatch,
  }) : super(key: key);

  final List<double> value;
  final List<ValueChanged<double>> onChanged;
  final List<String> parameterName;

  final bool bypass;
  final String name;

  final bool full;
  final Function() onCardTap;
  final Function() onBypassTap;

  final MaterialColor primarySwatch;

  Widget knobWithLabel(int index, double scaleFactor) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Knob(
          value: value[index],
          onChanged: full ? onChanged[index] : (_) {/* not interactive */},
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

    assert(false, 'Number of parameters must be between 1 and 4');
    return [];
  }

  Widget bypassButton(BuildContext context, double scaleFactor) {
    final button = Container(
      width: scaleFactor * 19,
      height: scaleFactor * 19,
      decoration: BoxDecoration(
        color: bypass
            ? Theme.of(context).colorScheme.surface
            : Theme.of(context).colorScheme.primary,
        shape: BoxShape.circle,
      ),
    );

    if (full) {
      return GestureDetector(
        onTap: full ? onBypassTap : () {/* not interactive */},
        child: button,
      );
    }

    return button;
  }

  @override
  Widget build(BuildContext context) {
    final scaleFactor = full ? 3.0 : 1.0;

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
            onTap: onCardTap,
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
                    child: bypassButton(context, scaleFactor),
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
