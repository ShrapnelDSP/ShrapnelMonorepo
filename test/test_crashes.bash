#!/bin/bash

for f in ./minimised/id*; do
    echo $f
    file $f
    hexdump -C $f
    ./fuzz/build/midi_mapping/json_parser_fuzz < $f
done
