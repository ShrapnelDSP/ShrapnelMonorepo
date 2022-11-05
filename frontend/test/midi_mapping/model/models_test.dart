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

import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:shrapnel/midi_mapping/model/models.dart';

void main() {
  test('Encode to json', () {
    const expected = '''
    {
      "messageType": "MidiMap::get::response",
      "mappings": {
        "1": { "midi_channel": 0, "cc_number": 1, "parameter_id": "gain" },
        "2": { "midi_channel": 0, "cc_number": 2, "parameter_id": "tone" }
      }
    }''';

    const message = MidiApiMessage.getResponse(
      mappings: {
        '1': MidiMapping(
          midiChannel: 0,
          ccNumber: 1,
          parameterId: 'gain',
        ),
        '2': MidiMapping(
          midiChannel: 0,
          ccNumber: 2,
          parameterId: 'tone',
        ),
      },
    );

    final actual = message.toJson();

    expect(actual, json.decode(expected));
  });

  test('Decode from json', () {
    const jsonString = '''
    {
      "messageType": "MidiMap::get::response",
      "mappings": {
        "1": { "midi_channel": 0, "cc_number": 1, "parameter_id": "gain" },
        "2": { "midi_channel": 0, "cc_number": 2, "parameter_id": "tone" }
      }
    }''';

    const expected = MidiApiMessage.getResponse(
      mappings: {
        '1': MidiMapping(
          midiChannel: 0,
          ccNumber: 1,
          parameterId: 'gain',
        ),
        '2': MidiMapping(
          midiChannel: 0,
          ccNumber: 2,
          parameterId: 'tone',
        ),
      },
    );

    final actual = MidiApiMessage.fromJson(
      json.decode(jsonString) as Map<String, dynamic>,
    );

    expect(actual, expected);
  });

  test('Decode from json with missing fields throws Error', () {
    const jsonString = '''
    {
      "messageType": "MidiMap::get::response",
      "mappings": {
        "1": { "cc_number": 1, "parameter_id": "gain" }
      }
    }''';

    expect(
      () => MidiApiMessage.fromJson(
        json.decode(jsonString) as Map<String, dynamic>,
      ),
      throwsA(const TypeMatcher<Error>()),
    );
  });

  test('Decode from json with missing values throws Error', () {
    const jsonString = '''
    {
      "messageType": "MidiMap::get::response",
      "mappings": {
        "1": { "midi_channel": null, "cc_number": 1, "parameter_id": "gain" }
      }
    }''';

    expect(
      () => MidiApiMessage.fromJson(
        json.decode(jsonString) as Map<String, dynamic>,
      ),
      throwsA(const TypeMatcher<Error>()),
    );
  });

  test('Create request', () {
    const requestJson = '''
    {
      "messageType": "MidiMap::create::request",
      "mapping": {
        "123": {
          "midi_channel": 0,
          "cc_number": 1,
          "parameter_id": "gain"
        }
      }
    }''';

    const request = MidiApiMessage.createRequest(
      mapping: MidiMappingEntry(
        id: '123',
        mapping: MidiMapping(
          midiChannel: 0,
          ccNumber: 1,
          parameterId: 'gain',
        ),
      ),
    );

    expect(request.toJson(), json.decode(requestJson));
    expect(
      MidiApiMessage.fromJson(
        json.decode(json.encode(request)) as Map<String, dynamic>,
      ),
      request,
    );
  });

  test('freezed as map value sanity test', () {
    final map = <String, MidiMapping>{};
    map['test'] =
        const MidiMapping(midiChannel: 5, ccNumber: 5, parameterId: 'test');
    expect(map.length, 1);
  });

  group('MappingEntry: ', () {
    test('encode and decode', () {
      const entryJson = '''
        {
          "123": {
            "midi_channel": 0,
            "cc_number": 1,
            "parameter_id": "gain"
          }
        }''';

      const entry = MidiMappingEntry(
        id: '123',
        mapping: MidiMapping(
          midiChannel: 0,
          ccNumber: 1,
          parameterId: 'gain',
        ),
      );

      expect(entry.toJson(), json.decode(entryJson));
      expect(
        MidiMappingEntry.fromJson(
          json.decode(json.encode(entry)) as Map<String, dynamic>,
        ),
        entry,
      );
    });

    test('throws if input is invalid', () {
      const entryJson = '''
        {
          "123": {
            "midi_channel": 0,
            "cc_number": 1,
            "parameter_id": "gain"
          },
          "456": {
            "midi_channel": 2,
            "cc_number": 3,
            "parameter_id": "volume"
          }
        }''';

      expect(
        () => MidiMappingEntry.fromJson(
          json.decode(entryJson) as Map<String, dynamic>,
        ),
        throwsA(isFormatException),
      );
    });
  });
}
