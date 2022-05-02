SharpnelDSP Firmware
====================

Build instructions
---

To build this firmware, use ESP-IDF v4.4, with the patch from this project's
[fork](https://github.com/ShrapnelDSP/esp-idf/commit/3df8e0cf7b5c1ee193f8444501ed8827134eacde).

This patch prevents warnings from headers included from other components,
and allows the project to use more warnings than the default in ESP-IDF. It is
also possible to build the project using unpatched ESP-IDF v4.4. In this case,
many false positive compiler warnings will appear.
