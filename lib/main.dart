import 'package:flutter/material.dart';

import 'stompbox.dart';
import 'valvestate.dart';
import 'parameter.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        brightness: Brightness.dark,
        primarySwatch: Colors.orange,
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatelessWidget {
  const MyHomePage({Key? key, required this.title}) : super(key: key);
  final String title;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(title),
      ),
      body: const ParameterChannelProvider(
        child: Center(
          child: Pedalboard(),
        ),
      ),
    );
  }
}

class Pedalboard extends StatefulWidget {
  const Pedalboard({Key? key}) : super(key: key);

  @override
  _PedalboardState createState() => _PedalboardState();
}

enum GearId {
  tubeScreamer,
  noiseGate,
  heavyMetal,
  chorus,
  valvestate,
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
            Stompbox(
              value: List.filled(3, _value),
              onChanged: List.filled(3, _setValue),
              parameterName: const ["DRIVE", "TONE", "LEVEL"],
              bypass: _bypass,
              name: "Tube Screamer",
              onTap: () => (_activateGear(GearId.tubeScreamer)),
              full: _activeGear == GearId.tubeScreamer,
              primarySwatch: Colors.green,
            ),
            Stompbox(
              value: List.filled(4, _value),
              onChanged: List.filled(4, _setValue),
              parameterName: const [
                "Threshold",
                "Hysteresis",
                "Attack",
                "Release"
              ],
              bypass: _bypass,
              name: "Noise Gate",
              onTap: () => (_activateGear(GearId.noiseGate)),
              full: _activeGear == GearId.noiseGate,
              primarySwatch: Colors.red,
            ),
            Stompbox(
              value: List.filled(4, _value),
              onChanged: List.filled(4, _setValue),
              parameterName: const ["LEVEL", "LOW", "HIGH", "DISTORTION"],
              bypass: _bypass,
              name: "Heavy Metal",
              onTap: () => _activateGear(GearId.heavyMetal),
              full: _activeGear == GearId.heavyMetal,
              primarySwatch: Colors.deepOrange,
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
            Stompbox(
              value: [_value, _value, _value],
              onChanged: List.filled(3, _setValue),
              parameterName: const ["RATE", "DEPTH", "MIX"],
              bypass: _bypass,
              name: "Chorus",
              onTap: () => (_activateGear(GearId.chorus)),
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
