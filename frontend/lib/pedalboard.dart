import 'package:flutter/material.dart';

import 'chorus.dart';
import 'heavy_metal.dart';
import 'noise_gate.dart';
import 'stompbox.dart';
import 'tube_screamer.dart';
import 'valvestate.dart';

class Pedalboard extends StatefulWidget {
  const Pedalboard({Key? key}) : super(key: key);

  @override
  _PedalboardState createState() => _PedalboardState();
}

class _PedalboardState extends State<Pedalboard> {
  double _value = 0;
  final bool _bypass = false;
  GearId? _activeGear;

  void _setValue(double value) => setState(() => _value = value);
  /*
  void _toggleBypass(int index) => setState(() => _bypass = !_bypass);
  */

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
                bypass: _bypass,
                full: _activeGear == GearId.tubeScreamer,
              ),
            ),
            NoiseGateParameterProvider(
              child: NoiseGate(
                onTap: () => _activateGear(GearId.noiseGate),
                bypass: _bypass,
                full: _activeGear == GearId.noiseGate,
              ),
            ),
            HeavyMetalParameterProvider(
              child: HeavyMetal(
                onTap: () => _activateGear(GearId.heavyMetal),
                bypass: _bypass,
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
                bypass: _bypass,
                full: _activeGear == GearId.chorus,
              ),
            ),
            Stompbox(
              value: [_value, _value, _value],
              onChanged: List.filled(3, _setValue),
              parameterName: const ['RATE', 'DEPTH', 'MIX'],
              bypass: _bypass,
              name: 'Chorus',
              onTap: () => _activateGear(GearId.chorus),
              full: _activeGear == GearId.chorus,
              primarySwatch: Colors.blue,
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
