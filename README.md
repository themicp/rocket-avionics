# Rocket Avionics

## Requirements
#### General
- Arduino IDE
- Arduino CLI (tested on version 0.11.0)
#### Ground Receiver
- Node.JS (tested on version 8+)
- InfluxDB 2.0.0

## Setup
Clone the repository with `git clone --recurse-submodules` to automatically download all the submodules.
#### Ground Receiver
- Copy `ground-receiver/message-handler/.env.template` to `ground-receiver/message-handler/.env` and fill it in with your configuration.
- Run `npm install` inside `ground-receiver/message-handler`
- Run `npm install` inside `ground-receiver/ground-controls`

## Deployment
**The deployment script is currently configured only for OS X.**

Example deploying the flight-computer on an Adafruit Feather M0 connected on the /dev/cu.usbmodem143301 port:
```
./deploy.sh flight-computer /dev/cu.usbmodem143301 adafruit:samd:adafruit_feather_m0
```

Example deploying the ground-receiver on an Arduino Uno connected on the /dev/cu.usbmodem143301 port:
```
./deploy.sh ground-receiver /dev/cu.usbmodem143301 arduino:avr:uno
```
