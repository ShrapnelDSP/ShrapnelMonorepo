#!/usr/bin/env bash

set -eo pipefail

proto_dir=$(pwd)
cd ../../../../proto
protoc --c_out="$proto_dir/generated" presets.proto