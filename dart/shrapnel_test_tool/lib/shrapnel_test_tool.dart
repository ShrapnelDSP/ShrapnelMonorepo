import 'package:logging/logging.dart';
import 'package:shrapnel_test_lib/shrapnel_test_lib.dart';

final _log = Logger('shrapnel.test_tool');

void main(List<String> arguments) {
  hierarchicalLoggingEnabled = true;
  Logger.root.onRecord.listen(print);

  _log.info('hello $arguments');
  nvsSave('nvs_${DateTime.now().toIso8601String()}.bin', port: '/dev/ttyUSB0');
}
