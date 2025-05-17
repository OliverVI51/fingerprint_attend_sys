#!/usr/bin/bash

SERIAL_PORT="/dev/ttyACM0"
TIME_STR=$(date +"%Y,%m,%d,%H,%M,%S")
echo "$TIME_STR" > "$SERIAL_PORT"

