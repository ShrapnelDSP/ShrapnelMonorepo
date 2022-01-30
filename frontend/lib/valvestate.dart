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
import 'parameter.dart';

class _ValvestateParameterGain extends AudioParameterDoubleModel {
  _ValvestateParameterGain({required ParameterService parameterService})
      : super(
          name: 'GAIN',
          id: 'ampGain',
          parameterService: parameterService,
        );
}

class _ValvestateParameterBass extends AudioParameterDoubleModel {
  _ValvestateParameterBass({required ParameterService parameterService})
      : super(
          name: 'BASS',
          id: 'bass',
          parameterService: parameterService,
        );
}

class _ValvestateParameterMiddle extends AudioParameterDoubleModel {
  _ValvestateParameterMiddle({required ParameterService parameterService})
      : super(
          name: 'MIDDLE',
          id: 'middle',
          parameterService: parameterService,
        );
}

class _ValvestateParameterTreble extends AudioParameterDoubleModel {
  _ValvestateParameterTreble({required ParameterService parameterService})
      : super(
          name: 'TREBLE',
          id: 'treble',
          parameterService: parameterService,
        );
}

class _ValvestateParameterContour extends AudioParameterDoubleModel {
  _ValvestateParameterContour({required ParameterService parameterService})
      : super(
          name: 'CONTOUR',
          id: 'contour',
          parameterService: parameterService,
        );
}

class _ValvestateParameterVolume extends AudioParameterDoubleModel {
  _ValvestateParameterVolume({required ParameterService parameterService})
      : super(
          name: 'VOLUME',
          id: 'volume',
          parameterService: parameterService,
        );
}

class Valvestate extends StatelessWidget {
  const Valvestate({
    Key? key,
    required this.full,
    required this.onTap,
  }) : super(key: key);

  final bool full;
  final void Function() onTap;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Consumer6<
            _ValvestateParameterGain,
            _ValvestateParameterBass,
            _ValvestateParameterMiddle,
            _ValvestateParameterTreble,
            _ValvestateParameterContour,
            _ValvestateParameterVolume>(
          builder: (context, gain, bass, middle, treble, contour, volume, _) =>
              Amplifier(
            parameter: [
              gain.value,
              bass.value,
              middle.value,
              treble.value,
              contour.value,
              volume.value,
            ],
            onChanged: [
              gain.onUserChanged,
              bass.onUserChanged,
              middle.onUserChanged,
              treble.onUserChanged,
              contour.onUserChanged,
              volume.onUserChanged,
            ],
            parameterName: [
              gain.name,
              bass.name,
              middle.name,
              treble.name,
              contour.name,
              volume.name,
            ],
            name: 'VALVESTATE 8100',
            onTap: onTap,
            full: full,
          ),
        ),
      ],
    );
  }
}

class ValvestateParameterProvider extends StatelessWidget {
  const ValvestateParameterProvider({
    Key? key,
    required this.child,
  }) : super(key: key);

  final Widget child;

  @override
  Widget build(BuildContext context) =>
      Consumer<ParameterService>(builder: (_, parameterService, __) {
        return MultiProvider(
          providers: [
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterGain(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterBass(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterMiddle(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterTreble(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterContour(
                parameterService: parameterService,
              ),
            ),
            ChangeNotifierProvider(
              create: (_) => _ValvestateParameterVolume(
                parameterService: parameterService,
              ),
            ),
          ],
          child: child,
        );
      });
}
