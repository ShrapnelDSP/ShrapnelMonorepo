#!/usr/bin/bash

flutter test
flutter analyze
dart format -o none --set-exit-if-changed lib test
