/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

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
