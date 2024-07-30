#!/usr/bin/bash

set -eo pipefail

flutter test --exclude-tags=api
flutter analyze
dart format -o none --set-exit-if-changed lib test
