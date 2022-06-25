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

import 'chorus.dart';
import 'heavy_metal.dart';
import 'noise_gate.dart';
import 'tube_screamer.dart';
import 'valvestate.dart';

class Pedalboard extends StatefulWidget {
  const Pedalboard({Key? key}) : super(key: key);

  @override
  _PedalboardState createState() => _PedalboardState();
}

class _PedalboardState extends State<Pedalboard> {
  GearId? _activeGear;

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            TubeScreamerParameterProvider(
              child: TubeScreamer(
                onTap: () => _activateGear(GearId.tubeScreamer),
                full: _activeGear == GearId.tubeScreamer,
              ),
            ),
            NoiseGate(
                    onTap: () => _activateGear(GearId.noiseGate),
                    full: _activeGear == GearId.noiseGate,
            ),
            HeavyMetalParameterProvider(
              child: HeavyMetal(
                onTap: () => _activateGear(GearId.heavyMetal),
                full: _activeGear == GearId.heavyMetal,
              ),
            ),
          ],
        ),
        Valvestate(
          onTap: () => _activateGear(GearId.valvestate),
          full: _activeGear == GearId.valvestate,
        ),
        ChorusParameterProvider(
          child: Chorus(
            onTap: () => _activateGear(GearId.chorus),
            full: _activeGear == GearId.chorus,
          ),
        ),
      ],
    );
  }

  void _activateGear(GearId gear) => setState(() {
        if (_activeGear == gear) {
          _activeGear = null;
          return;
        }

        _activeGear = gear;
      });
}

enum GearId {
  tubeScreamer,
  noiseGate,
  heavyMetal,
  chorus,
  valvestate,
}
