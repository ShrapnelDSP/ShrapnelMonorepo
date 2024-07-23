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

import 'dart:async';

abstract class MessageTransport<T1, T2> {
  /// Messages added to this sink will be sent to the other side of the
  /// connection.
  StreamSink<T1> get sink;

  /// Messages from the other side of the connection will appear in this stream.
  Stream<T2> get stream;

  /// Must be called to clean up resource after the transport is no longer in
  /// use.
  void dispose();
}

abstract class ReconnectingMessageTransport<T1, T2>
    extends MessageTransport<T1, T2> {
  /// Returns true if the connection is alive at the moment
  bool get isAlive;
}
