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

// ignore_for_file: unnecessary_raw_strings

import 'package:shrapnel_test_lib/shrapnel_test_lib.dart';
import 'package:test/test.dart';

void main() {
  test('escape spaces', () {
    final testCases = <String, String>{
      r'abcdef': r'abcdef',
      r'abc def': r'abc\ def',
      r'a b c d e f': r'a\ b\ c\ d\ e\ f',
      r'a   b': r'a\ \ \ b',
      r'abc\def': r'abc\\def',
      r'a\b\c\\\def': r'a\\b\\c\\\\\\def',
      r'a"b': r'a\"b',
      r'a"b"""': r'a\"b\"\"\"',
    };

    final result =
        testCases.map((key, value) => MapEntry(key, escapeCommandArg(key)));

    expect(result, testCases);
  });
}
