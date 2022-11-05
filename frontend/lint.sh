#!/usr/bin/bash

flutter test
flutter analyze
flutter format -o none --set-exit-if-changed lib test
