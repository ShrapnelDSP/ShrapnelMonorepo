#!/bin/env bash

find effects firmware plugins test -regex '.*\.\(cpp\|hpp\|c\|h\)' -not -regex ".*\(/build\|/cmake-build\|test/support\|.venv/\|firmware/managed_components\).*"
