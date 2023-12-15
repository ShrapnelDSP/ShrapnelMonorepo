#!/usr/bin/env bash

set -eo pipefail

proto_dir=$(pwd)
cd ../../../../proto
protoc --dart_out="$proto_dir/generated" presets.proto
