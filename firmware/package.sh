#!/usr/bin/env bash
idf.py build
cp build/bootloader/bootloader.bin build/partition_table/partition-table.bin build/esp32-dsp.bin dist
