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
            NoiseGateParameterProvider(
              child: NoiseGate(
                onTap: () => _activateGear(GearId.noiseGate),
                full: _activeGear == GearId.noiseGate,
              ),
            ),
            HeavyMetalParameterProvider(
              child: HeavyMetal(
                onTap: () => _activateGear(GearId.heavyMetal),
                full: _activeGear == GearId.heavyMetal,
              ),
            ),
          ],
        ),
        ValvestateParameterProvider(
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Valvestate(
                onTap: () => _activateGear(GearId.valvestate),
                full: _activeGear == GearId.valvestate,
              ),
            ],
          ),
        ),
        Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            ChorusParameterProvider(
              child: Chorus(
                onTap: () => _activateGear(GearId.chorus),
                full: _activeGear == GearId.chorus,
              ),
            ),
          ],
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
