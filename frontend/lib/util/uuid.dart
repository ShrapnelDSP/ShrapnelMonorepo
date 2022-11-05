import 'package:flutter/widgets.dart';
import 'package:uuid/uuid.dart' as uuid;

class Uuid extends ChangeNotifier {
  final _uuid = const uuid.Uuid();

  /// Returns a v4 (random) UUID
  String v4() => _uuid.v4();
}
