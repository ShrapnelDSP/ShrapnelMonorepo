import 'package:flutter/material.dart';
import 'knob.dart';
import 'stompbox.dart';

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
              children: <Widget>[
                Stompbox(
                  value: [_value],
                  onChanged: List.filled(1, _setValue),
                  bypass: _bypass,
                  name: "one",
                ),
                Stompbox(
                  value: [_value, _value],
                  onChanged: List.filled(2, _setValue),
                  bypass: _bypass,
                  name: "two",
                ),
                Stompbox(
                  value: [_value, _value, _value],
                  onChanged: List.filled(3, _setValue),
                  bypass: _bypass,
                  name: "three",
                ),
                Stompbox(
                  value: [_value, _value, _value, _value],
                  onChanged: List.filled(4, _setValue),
                  bypass: _bypass,
                  name: "stomp",
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
