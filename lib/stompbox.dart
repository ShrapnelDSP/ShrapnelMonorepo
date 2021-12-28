import 'package:flutter/material.dart';
import 'knob.dart';

class Stompbox extends StatelessWidget {
    final List<double> value;
    final bool bypass;
    final String name;

    const Stompbox({
        Key? key,
        required this.value,
        required this.bypass,
        required this.name,
    }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 100,
      height: 150,
      child: Card(
        child: Container(
          margin: const EdgeInsets.all(10),
          child: Stack(
            alignment: Alignment.center,
            children: <Widget>[
              Positioned(
                top: 70,
                child: Text(name),
              ),
              Positioned(
                left: 0,
                top: 0,
                child: Knob(
                  value: value[0],
                  onChanged: (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: 25,
                ),
              ),
              Positioned(
                right: 0,
                top: 0,
                child: Knob(
                  value: value[1],
                  onChanged: (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: 25,
                ),
              ),
              Positioned(
                left: 0,
                top: 35,
                child: Knob(
                  value: value[2],
                  onChanged: (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: 25,
                ),
              ),
              Positioned(
                right: 0,
                top: 35,
                child: Knob(
                  value: value[3],
                  onChanged: (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: 25,
                ),
              ),
              Positioned(
                top: 95,
                child: Container(
                  width:25,
                  height:25,
                  decoration: BoxDecoration(
                    color: Theme.of(context).colorScheme.background,
                    shape: BoxShape.circle,
                  ),
                ),
              ),
              Positioned(
                top: 98,
                child: Container(
                  width:19,
                  height:19,
                  decoration: BoxDecoration(
                    color: bypass ? Theme.of(context).colorScheme.surface : Theme.of(context).colorScheme.primary,
                    shape: BoxShape.circle,
                  ),
                ),
              ),
            ]
          ),
        ),
      ),
    );
  }
}
