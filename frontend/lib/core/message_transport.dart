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
