#!/bin/bash

mkdir -p minimised
for f in ./fuzz/output-batch-2/**/crashes/id*; do
    echo $f
    afl-tmin -i $f -o minimised/$(basename $f) -- ./fuzz/build/midi_mapping/json_parser_fuzz
done
