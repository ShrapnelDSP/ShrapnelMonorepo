import 'package:integration_test/integration_test.dart';

import '../test/midi_mapping/view/midi_mapping_test.dart' as test;

void main() {
  IntegrationTestWidgetsFlutterBinding.ensureInitialized();
  test.main();
}