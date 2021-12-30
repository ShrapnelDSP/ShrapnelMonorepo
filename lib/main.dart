import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

import 'amplifier.dart';
import 'knob.dart';
import 'stompbox.dart';
import 'valvestate.dart';

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
  bool _bypass = false;

  void _setValue(double value) => setState(() => _value = value);
  void _toggleBypass(int index) => setState(() => _bypass = !_bypass);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                Stompbox(
                  value: List.filled(3, _value),
                  onChanged: List.filled(3, _setValue),
                  parameterName: ["DRIVE", "TONE", "LEVEL"],
                  bypass: _bypass,
                  name: "Tube Screamer",
                  primarySwatch: Colors.green,
                ),
                Stompbox(
                  value: List.filled(4, _value),
                  onChanged: List.filled(4, _setValue),
                  parameterName: [
                    "Threshold",
                    "Hysteresis",
                    "Attack",
                    "Release"
                  ],
                  bypass: _bypass,
                  name: "Noise Gate",
                  primarySwatch: Colors.red,
                ),
                Stompbox(
                  value: List.filled(4, _value),
                  onChanged: List.filled(4, _setValue),
                  parameterName: ["LEVEL", "LOW", "HIGH", "DISTORTION"],
                  bypass: _bypass,
                  name: "Heavy Metal",
                  primarySwatch: Colors.deepOrange,
                ),
              ],
            ),
            ChangeNotifierProvider(
              create: (_) => ValvestateParameterGain(),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
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
                  parameterName: ["RATE", "DEPTH", "MIX"],
                  bypass: _bypass,
                  name: "Chorus",
                  primarySwatch: Colors.blue,
                ),
              ],
            )
          ],
        ),
      ),
    );
  }
}
