# ShrapnelDSP

[![Coverage Status](https://coveralls.io/repos/github/ShrapnelDSP/ShrapnelMonorepo/badge.svg)](https://coveralls.io/github/ShrapnelDSP/ShrapnelMonorepo)
[![Firmware docs](https://img.shields.io/badge/docs-firmware-blue)](https://shrapneldsp.github.io/ShrapnelMonorepo/firmware-docs/)
[![Frontend docs](https://img.shields.io/badge/docs-frontend-blue)](https://shrapneldsp.github.io/ShrapnelMonorepo/frontend-docs/)
[![Requirements](https://img.shields.io/badge/docs-requirements-blue)](https://shrapneldsp.github.io/ShrapnelMonorepo/requirements/)

# Running integration tests

Prerequisites:

1. Set up Wi-Fi network for testing:
    * Must use 2.4 GHz band, ShrapnelDSP device has no 5 GHz support.
    * Ethernet connection to PC running the test so it stays online while connecting to the
      ShrapnelDSP access point for provisioning.
    * Static IP address set up for the ShrapnelDSP device.
2. Set up the development machine for running integration tests:
    * [NetworkManager](https://wiki.archlinux.org/title/NetworkManager) and the nmcli tool is is
      required to connect to the ShrapnelDSP device for provisioning.
    * Create a NetworkManager connection profile for the ShrapnelDSP device's access point. The name
      of the profile must be exactly the same as the access point's SSID. This can be done easily by
      connecting to the access point using nmtui. It creates a profile with the correct settings by
      default.
    * Create a test_config.json file at the root of the repository. This must be a JSON file
      containing a single object with keys DUT_IP_ADDRESS, NETWORK_SSID, NETWORK_PASSPHRASE,
      FIRMWARE_BINARY_PATH. The network credentials are used during Wi-Fi provisioning. The firmware
      binary path should contain bootloader.bin, partition-table.bin and esp32-dsp.bin files.

To execute tests, `cd frontend`, then use `run_api_test.sh` or `run_integration_test.sh`.
