#!/bin/bash

pio run --target upload --environment teensy36
sleep 1
pio device monitor --port /dev/ttyACM0
