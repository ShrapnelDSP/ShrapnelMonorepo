// ignore_for_file: unnecessary_raw_strings

import 'package:flutter_test/flutter_test.dart';

import 'util.dart';

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
