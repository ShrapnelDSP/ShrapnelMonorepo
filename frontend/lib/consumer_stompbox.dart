/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'knob_with_label.dart';
import 'parameter.dart';
import 'util/conditional_parent.dart';

class StompboxModel extends ChangeNotifier {
  StompboxModel({required this.parameters, required this.bypass});

  List<AudioParameterDoubleModel> parameters;
  AudioParameterDoubleModel bypass;
}

class ConsumerStompbox extends StatelessWidget {
  const ConsumerStompbox({
    Key? key,
    required this.name,
    required this.full,
    required this.onCardTap,
    required this.primarySwatch,
  }) : super(key: key);

  final String name;

  final bool full;
  final Function() onCardTap;

  final MaterialColor primarySwatch;

  List<Widget> knobs(BuildContext context, double scaleFactor) {
    final layouts = <int, List<_KnobPosition>>{
      1: [const _KnobPosition(top: 0)],
      2: [
        const _KnobPosition(left: 0, top: 0),
        const _KnobPosition(right: 0, top: 0),
      ],
      3: [
        const _KnobPosition(left: 0, top: 0),
        const _KnobPosition(right: 0, top: 0),
        _KnobPosition(top: 35 * scaleFactor),
      ],
      4: [
        const _KnobPosition(left: 0, top: 0),
        const _KnobPosition(right: 0, top: 0),
        _KnobPosition(left: 0, top: 35 * scaleFactor),
        _KnobPosition(right: 0, top: 35 * scaleFactor),
      ],
    };

    final parameters = Provider.of<StompboxModel>(context).parameters;
    assert(layouts.containsKey(parameters.length),
        'Number of parameters must be one of ${layouts.keys}, not ${parameters.length}');

    final layout = layouts[parameters.length]!;

    return List<Widget>.generate(
        parameters.length,
        (index) => Positioned(
              top: layout[index].top,
              left: layout[index].left,
              right: layout[index].right,
              child: ChangeNotifierProvider.value(
                value: parameters[index],
                child: KnobWithLabel(
                  isEnabled: full,
                  knobSize: scaleFactor * 25,
                ),
              ),
            ),
        growable: false);
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
                child: Stack(
                  alignment: Alignment.center,
                  children: <Widget>[
                    ...knobs(context, scaleFactor),
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
                      child: ChangeNotifierProvider.value(
                        value: context.watch<StompboxModel>().bypass,
                        child: _BypassButton(
                          size: scaleFactor * 19,
                          isEnabled: full,
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class _KnobPosition {
  const _KnobPosition({this.top, this.left, this.right});

  final double? top;
  final double? left;
  final double? right;
}

class _BypassButton extends StatelessWidget {
  const _BypassButton({Key? key, required this.size, required this.isEnabled})
      : super(key: key);

  final double size;
  final bool isEnabled;

  @override
  Widget build(BuildContext context) {
    final bypass = context.watch<AudioParameterDoubleModel>();

    return ConditionalParent(
      condition: isEnabled,
      builder: (child) => GestureDetector(
        onTap: () {
          bypass.onUserChanged((bypass.value > 0.5) ? 0 : 1);
        },
        child: child,
      ),
      child: Container(
        width: size,
        height: size,
        decoration: BoxDecoration(
          color: bypass.value > 0.5
              ? Theme.of(context).colorScheme.surface
              : Theme.of(context).colorScheme.primary,
          shape: BoxShape.circle,
        ),
      ),
    );
  }
}
