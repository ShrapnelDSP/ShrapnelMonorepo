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

import 'amplifier.dart';
import 'knob_with_label.dart';
import 'midi_mapping/model/midi_learn_state.dart';
import 'parameter.dart';
import 'util/conditional_parent.dart';

abstract class StompboxModel {
  String get name;
  List<AudioParameterDoubleModel> get parameters;
  AudioParameterDoubleModel get bypass;
}

class Stompbox extends StatelessWidget {
  const Stompbox({
    super.key,
    required this.model,
    required this.full,
    required this.onCardTap,
    required this.primarySwatch,
  });

  final bool full;
  final void Function() onCardTap;
  final StompboxModel model;

  final MaterialColor primarySwatch;

  List<Widget> knobs(BuildContext context, double scaleFactor) {
    final parameters = model.parameters;

    return List<Widget>.generate(
      (parameters.length + 1) ~/ 2,
      (i) {
        return Row(
          children: List<Widget>.generate(
            2 * i + 1 >= parameters.length ? 1 : 2,
            (j) {
              return parameters[2 * i + j].provider(
                child: Expanded(
                  child: Padding(
                    padding: EdgeInsets.only(bottom: scaleFactor * 3),
                    child: KnobWithLabel(
                      isEnabled: full,
                      knobSize: scaleFactor * 25,
                    ),
                  ),
                ),
              );
            },
            growable: false,
          ),
        );
      },
      growable: false,
    );
  }

  @override
  Widget build(BuildContext context) {
    final scaleFactor = full ? 3.0 : 1.0;
    final name = model.name;

    return Theme(
      data: Theme.of(context).copyWith(
        colorScheme: Theme.of(context).colorScheme.copyWith(
              primary: primarySwatch.shade400,
            ),
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
              key: Key('stompbox-$name'),
              child: Padding(
                padding: EdgeInsets.all(scaleFactor * 8),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: <Widget>[
                    ...knobs(context, scaleFactor),
                    Expanded(
                      child: Center(
                        child: Text(
                          name,
                          style: DefaultTextStyle.of(context)
                              .style
                              .apply(fontSizeFactor: scaleFactor),
                          textAlign: TextAlign.center,
                        ),
                      ),
                    ),
                    model.bypass.provider(
                      child: _BypassButton(
                        size: scaleFactor * 23,
                        isEnabled: full,
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

class _BypassButton extends StatelessWidget {
  const _BypassButton({required this.size, required this.isEnabled});

  final double size;
  final bool isEnabled;

  @override
  Widget build(BuildContext context) {
    final value = context.watch<double>();
    final bypass = context.read<AudioParameterDoubleModel>();
    final learningState = context.watch<MidiLearnState>();

    return ConditionalParent(
      condition: isEnabled,
      builder: (child) => GestureDetector(
        onTap: () {
          bypass.onUserChanged((value > 0.5) ? 0 : 1);
        },
        child: child,
      ),
      child: Theme(
        data: Theme.of(context).copyWith(
          // TODO could create a new derived notifier of String? of the active
          //  learning ID. This would be easier to update everywhere in case we
          //  have additional states where mapping is still active.
          colorScheme: learningState.maybeWhen(
            waitForMidi: (id) => id == bypass.id
                ? Theme.of(context)
                    .colorScheme
                    .copyWith(background: Colors.white, primary: Colors.white)
                : null,
            orElse: () => null,
          ),
        ),
        child: Builder(
          builder: (context) {
            return Container(
              width: size,
              height: size,
              decoration: BoxDecoration(
                color: Theme.of(context).colorScheme.background,
                shape: BoxShape.circle,
              ),
              child: Center(
                child: Container(
                  width: size * 0.8,
                  height: size * 0.8,
                  decoration: BoxDecoration(
                    color: value > 0.5
                        ? Theme.of(context).colorScheme.surface
                        : Theme.of(context).colorScheme.primary,
                    shape: BoxShape.circle,
                  ),
                ),
              ),
            );
          },
        ),
      ),
    );
  }
}
