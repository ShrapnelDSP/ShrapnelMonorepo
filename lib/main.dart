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

class MyHomePage extends StatefulWidget {
  const MyHomePage({Key? key, required this.title}) : super(key: key);
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  double _value = 0;
  final bool _bypass = false;
  bool _stompboxFull = false;

  void _setValue(double value) => setState(() => _value = value);
  /*
  void _toggleBypass(int index) => setState(() => _bypass = !_bypass);
  */
  void _stompboxTapped() => setState(() =>_stompboxFull = !_stompboxFull);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: ParameterChannelProvider(
        child: Center(
          child: Column(
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
                    onTap: _stompboxTapped,
                    full: _stompboxFull,
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
                    onTap: _stompboxTapped,
                    full: _stompboxFull,
                    primarySwatch: Colors.red,
                  ),
                  Stompbox(
                    value: List.filled(4, _value),
                    onChanged: List.filled(4, _setValue),
                    parameterName: const ["LEVEL", "LOW", "HIGH", "DISTORTION"],
                    bypass: _bypass,
                    name: "Heavy Metal",
                    onTap: _stompboxTapped,
                    full: _stompboxFull,
                    primarySwatch: Colors.deepOrange,
                  ),
                ],
              ),
              ValvestateParameterProvider(
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: const <Widget>[
                    Valvestate(),
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
                    onTap: _stompboxTapped,
                    full: _stompboxFull,
                    primarySwatch: Colors.blue,
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }
}
