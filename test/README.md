# Testing

## Unit tests

```

```

## Fuzz tests

```
cmake -B fuzz/build -DCMAKE_TOOLCHAIN_FILE=afl_toolchain.cmake
cmake --build fuzz/build/
afl -i fuzz/input -o fuzz/output ./fuzz/build/midi_mapping/json_parser_fuzz
```
