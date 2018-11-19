#!/bin/bash

pio run --target upload
sleep 1
pio device monitor --port /dev/ttyACM0
