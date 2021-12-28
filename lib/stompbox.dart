import 'package:flutter/material.dart';
import 'knob.dart';

class Stompbox extends StatefulWidget {
    final List<double> value;
    final List<ValueChanged<double>> onChanged;

    final bool bypass;
    final String name;

    const Stompbox({
        Key? key,
        required this.value,
        required this.onChanged,
        required this.bypass,
        required this.name,
    }) : super(key: key);

    @override
    _StompboxState createState() => _StompboxState();
}

class _StompboxState extends State<Stompbox> {
  bool _full = false;

  List<Widget> knobs(double scaleFactor) {
      if(widget.value.length == 1)
      {
          return [ Positioned(
                top: 0,
                child: Knob(
                  value: widget.value[0],
                  onChanged: _full ? widget.onChanged[0] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 50,
                ),
              ),

          ];
      }
      else if(widget.value.length == 2)
      {
          return [ Positioned(
                left: 0,
                top: 0,
                child: Knob(
                  value: widget.value[0],
                  onChanged: _full ? widget.onChanged[0] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
              Positioned(
                right: 0,
                top: 0,
                child: Knob(
                  value: widget.value[1],
                  onChanged: _full ? widget.onChanged[1] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),

          ];
      }
      else if(widget.value.length == 3)
      {
          return [ Positioned(
                left: 0,
                top: 0,
                child: Knob(
                  value: widget.value[0],
                  onChanged: _full ? widget.onChanged[0] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
              Positioned(
                right: 0,
                top: 0,
                child: Knob(
                  value: widget.value[1],
                  onChanged: _full ? widget.onChanged[1] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
              Positioned(
                top: scaleFactor * 35,
                child: Knob(
                  value: widget.value[2],
                  onChanged: _full ? widget.onChanged[2] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
          ];
      }
      else if(widget.value.length == 4)
      {
          return [ Positioned(
                left: 0,
                top: 0,
                child: Knob(
                  value: widget.value[0],
                  onChanged: _full ? widget.onChanged[0] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
              Positioned(
                right: 0,
                top: 0,
                child: Knob(
                  value: widget.value[1],
                  onChanged: _full ? widget.onChanged[1] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
              Positioned(
                left: 0,
                top: scaleFactor * 35,
                child: Knob(
                  value: widget.value[2],
                  onChanged: _full ? widget.onChanged[2] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
              Positioned(
                right: 0,
                top: scaleFactor * 35,
                child: Knob(
                  value: widget.value[3],
                  onChanged: _full ? widget.onChanged[3] : (ignored) {/* not interactive */},
                  min: 0,
                  max: 10,
                  size: scaleFactor * 25,
                ),
              ),
        ];
      }

      assert(false, "Number of parameters must be between 1 and 4");
      return [];
  }

  void _toggleSize() => setState( () => _full = !_full);

  @override
  Widget build(BuildContext context) {
    double scaleFactor = _full ? 3 : 1;

    return SizedBox(
      width: scaleFactor * 100,
      height: scaleFactor * 150,
      child: GestureDetector(
      onTap: _toggleSize,
      child: Card(
        child: Container(
          margin: EdgeInsets.all(scaleFactor * 10),
          child: Stack(
            alignment: Alignment.center,
            children: <Widget>[
              ...knobs(scaleFactor),
              Positioned(
                top: scaleFactor * 70,
                child: Text(widget.name),
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
                    color: widget.bypass ? Theme.of(context).colorScheme.surface : Theme.of(context).colorScheme.primary,
                    shape: BoxShape.circle,
                  ),
                ),
              ),
            ]
          ),
        ),
      ),
      ),
    );
  }
}
