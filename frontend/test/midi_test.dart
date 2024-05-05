import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/api/generated/midi_mapping.pb.dart' as pb;
import 'package:shrapnel/api/proto_extension.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';

void main() {
  test('encode midi', () {
    final onMessage =
        MidiMessage.controlChange(channel: 0, control: 1, value: 0xFF);
    final offMessage =
        MidiMessage.controlChange(channel: 0, control: 1, value: 0x00);

    print('on: ${base64Encode(onMessage.toProto().writeToBuffer())}');
    print('off: ${base64Encode(offMessage.toProto().writeToBuffer())}');
  });
}
