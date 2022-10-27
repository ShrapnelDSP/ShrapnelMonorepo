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
      "mappings": {
        "1": { "midi_channel": 0, "cc_number": 1, "parameter_id": "gain" },
        "2": { "midi_channel": 0, "cc_number": 2, "parameter_id": "tone" }
      }
    }''';

    const message = GetResponse(
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

    final actual = json.decode(json.encode(message)) as Map<String, dynamic>;

    expect(actual, json.decode(expected));
  });

  test('Decode from json', () {
    const jsonString = '''
    {
      "mappings": {
        "1": { "midi_channel": 0, "cc_number": 1, "parameter_id": "gain" },
        "2": { "midi_channel": 0, "cc_number": 2, "parameter_id": "tone" }
      }
    }''';

    const expected = GetResponse(
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

    final actual =
        GetResponse.fromJson(json.decode(jsonString) as Map<String, dynamic>);

    expect(actual, expected);
  });

  test('Decode from json with missing fields throws Error', () {
    const jsonString = '''
    {
      "mappings": {
        "1": { "cc_number": 1, "parameter_id": "gain" }
      }
    }''';

    expect(
        () => GetResponse.fromJson(
            json.decode(jsonString) as Map<String, dynamic>),
        throwsA(const TypeMatcher<Error>()));
  });

  test('Decode from json with missing values throws Error', () {
    const jsonString = '''
    {
      "mappings": {
        "1": { "midi_channel": null, "cc_number": 1, "parameter_id": "gain" }
      }
    }''';

    expect(
        () => GetResponse.fromJson(
            json.decode(jsonString) as Map<String, dynamic>),
        throwsA(const TypeMatcher<Error>()));
  });
}
