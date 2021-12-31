import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

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

  final _channel = WebSocketChannel.connect(
    Uri.parse('ws://guitar-dsp.local/websocket'),
  );

  void _setValue(double value) => setState(() => _value = value);
  /*
  void _toggleBypass(int index) => setState(() => _bypass = !_bypass);
  */

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
                  parameterName: const ["DRIVE", "TONE", "LEVEL"],
                  bypass: _bypass,
                  name: "Tube Screamer",
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
                  primarySwatch: Colors.red,
                ),
                Stompbox(
                  value: List.filled(4, _value),
                  onChanged: List.filled(4, _setValue),
                  parameterName: const ["LEVEL", "LOW", "HIGH", "DISTORTION"],
                  bypass: _bypass,
                  name: "Heavy Metal",
                  primarySwatch: Colors.deepOrange,
                ),
              ],
            ),
            MultiProvider(
              providers: [
                ChangeNotifierProvider(
                  create: (_) => ValvestateParameterGain(),
                ),
                ChangeNotifierProvider(
                  create: (_) => ValvestateParameterBass(),
                ),
                ChangeNotifierProvider(
                  create: (_) => ValvestateParameterMiddle(),
                ),
                ChangeNotifierProvider(
                  create: (_) => ValvestateParameterTreble(),
                ),
                ChangeNotifierProvider(
                  create: (_) => ValvestateParameterContour(),
                ),
                ChangeNotifierProvider(
                  create: (_) => ValvestateParameterVolume(),
                ),
              ],
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  Valvestate(channel: _channel),
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
                  primarySwatch: Colors.blue,
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    _channel.sink.close();
    super.dispose();
  }
}
