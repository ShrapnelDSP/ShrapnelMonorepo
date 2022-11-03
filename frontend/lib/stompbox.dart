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

class Stompbox extends StatelessWidget {
  const Stompbox({
    super.key,
    required this.name,
    required this.full,
    required this.onCardTap,
    required this.primarySwatch,
  });

  final String name;

  final bool full;
  final Function() onCardTap;

  final MaterialColor primarySwatch;

  List<Widget> knobs(BuildContext context, double scaleFactor) {
    final parameters = Provider.of<StompboxModel>(context).parameters;

    return List<Widget>.generate(
      (parameters.length + 1) ~/ 2,
      (i) {
        return Row(
          children: List<Widget>.generate(
            2 * i + 1 >= parameters.length ? 1 : 2,
            (j) {
              return ChangeNotifierProvider.value(
                value: parameters[2 * i + j],
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
                    ),),),
                    ChangeNotifierProvider.value(
                      value: context.watch<StompboxModel>().bypass,
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
          color: Theme.of(context).colorScheme.background,
          shape: BoxShape.circle,
        ),
        child: Center(
          child: Container(
            width: size * 0.8,
            height: size * 0.8,
            decoration: BoxDecoration(
              color: bypass.value > 0.5
                  ? Theme.of(context).colorScheme.surface
                  : Theme.of(context).colorScheme.primary,
              shape: BoxShape.circle,
            ),
          ),
        ),
      ),
    );
  }
}
