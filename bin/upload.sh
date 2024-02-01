#!/bin/bash

~/apps/arduino-cli/bin/arduino-cli upload -p /dev/ttyACM0 -b arduino:samd:nano_33_iot -i .pio/build/nano_33_iot/firmware.bin
