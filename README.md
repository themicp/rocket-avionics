# Rocket Avionics

## Requirements
- Arduino IDE
- Arduino CLI (tested on version 0.11.0)

## Setup
Clone the repository with `git clone --recurse-submodules` to automatically download all the submodules.

## Deployment
**Currently works only on OS X.**

Example deploying the flight-computer on an Adafruit Feather M0 connected on the /dev/cu.usbmodem143301 port:
```
./deploy.sh flight-computer /dev/cu.usbmodem143301 adafruit:samd:adafruit_feather_m0
```

Example deploying the ground-receiver on an Arduino Uno connected on the /dev/cu.usbmodem143301 port:
```
./deploy.sh ground-receiver /dev/cu.usbmodem143301 arduino:avr:uno
```
