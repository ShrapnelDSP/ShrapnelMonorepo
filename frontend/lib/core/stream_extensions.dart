import 'package:logging/logging.dart';

extension LogEx<T> on Stream<T> {
  /// Run the callback for each element, but otherwise has no effect on the
  /// stream
  Stream<T> sideEffect(void Function(T event) callback) {
    return map((event) {
      callback(event);
      return event;
    });
  }

  Stream<T> logInfo(Logger log, String Function(T event) format) {
    return sideEffect((event) {
      log.info(format(event));
    });
  }

  Stream<T> logFine(Logger log, String Function(T event) format) {
    return sideEffect((event) {
      log.fine(format(event));
    });
  }

  Stream<T> logFiner(Logger log, String Function(T event) format) {
    return sideEffect((event) {
      log.finer(format(event));
    });
  }

  Stream<T> logFinest(Logger log, String Function(T event) format) {
    return sideEffect((event) {
      log.finest(format(event));
    });
  }
}
