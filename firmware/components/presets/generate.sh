#!/usr/bin/env bash

set -eo pipefail

cd proto
protoc --c_out=generated presets.proto