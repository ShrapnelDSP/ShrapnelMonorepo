#!/usr/bin/env bash

set -eo pipefail

proto_dir=$(pwd)
cd ../../../../proto
protoc --dart_out="$proto_dir/generated" \
  audio_events.proto \
  cmd_handling.proto \
  midi_mapping.proto \
  presets.proto \
  selected_preset.proto \
  shrapnel.proto
