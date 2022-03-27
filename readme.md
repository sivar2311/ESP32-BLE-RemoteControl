[![Build](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml/badge.svg)](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml)
[![Release](https://img.shields.io/github/v/release/sivar2311/ESP32-BLE-RemoteControl)](https://github.com/sivar2311/ESP32-BLE-RemoteControl/releases)
# ESP32-BLE-Remote-Control

This library allows you to send and receive simple command messages via Bluetooth Low Energy.

# Features:

## Connectionless

Messages are sent as Bluetooth advertisement packets - no active connection or pairing is required.

## Security
- 128bit AES encryption: All messages are transmitted encrypted using 128bit AES encryption.
- NONCE: Each message contains a random NONCE value that increases the entropy of the encrypted message.
- Rolling code: Each message is provided with a rolling code to prevent replay attacks.

## One receiver - multiple senders
A receiver can (theoretically) manage up to 65,536 different remote controls and process 4,294,967,296 different or 32 simultaneous commands (the interpretation is left to the application / user).

# Installation

Currently this library is neither registered with Arduino nor PlatformIO and therefore has to be installed / included manually. 

### Arduino IDE

Download the repository as a ZIP file.
Then select `Sketch` / `Include Library` / `Add .ZIP library...` in the ArduinoIDE menu and select the downloaded ZIP file.

### PlatformIO

Add the following to your `platformio.ini`:
```ini
lib_deps = 
  https://github.com/sivar2311/ESP32-BLE-RemoteControl.git
``` 

## Dependencies

This library depends on [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).

# Use

This chapter needs to be recreated as there have been fundamental changes.
Until then, please have a look at the [examples](https://github.com/sivar2311/ESP32-BLE-RemoteControl/tree/master/examples).