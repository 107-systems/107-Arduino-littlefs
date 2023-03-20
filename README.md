<a href="https://107-systems.org/"><img align="right" src="https://raw.githubusercontent.com/107-systems/.github/main/logo/107-systems.png" width="15%"></a>
:floppy_disk: `107-Arduino-24LCxx-littlefs`
===========================================
[![Arduino Library Badge](https://www.ardu-badge.com/badge/107-Arduino-24LCxx-littlefs.svg?)](https://www.ardu-badge.com/107-Arduino-24LCxx-littlefs)
[![Compile Examples](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/workflows/Compile%20Examples/badge.svg)](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/actions?workflow=Compile+Examples)
[![Arduino Lint](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/workflows/Arduino%20Lint/badge.svg)](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/actions?workflow=Arduino+Lint)
[![Sync Labels](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/workflows/Sync%20Labels/badge.svg)](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/actions?workflow=Sync+Labels)
[![keywords.txt Checks](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/workflows/Extra%20Library%20Checks/badge.svg)](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/actions?workflow=Extra+Library+Checks)
[![General Formatting Checks](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/workflows/General%20Formatting%20Checks/badge.svg)](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/actions?workflow=General+Formatting+Checks)
[![Spell Check](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/workflows/Spell%20Check/badge.svg)](https://github.com/107-systems/107-Arduino-24LCxx-littlefs/actions?workflow=Spell+Check)

This Arduino library provides [littlefs](https://github.com/littlefs-project/littlefs) on top of 24LCxx EEPROM's.

Such a feature is useful for the [Cyphal](https://opencyphal.org/) protocol and it's Arduino port [107-Arduino-Cyphal](https://github.com/107-systems/107-Arduino-Cyphal) but naturally can also be used in another context.

<p align="center">
  <a href="https://github.com/107-systems/l3xz"><img src="https://raw.githubusercontent.com/107-systems/.github/main/logo/l3xz-logo-memento-mori-github.png" width="40%"></a>
</p>

This library works for
* [ArduinoCore-samd](https://github.com/arduino/ArduinoCore-samd): [`Arduino Zero`](https://store.arduino.cc/arduino-zero), [`MKR 1000`](https://store.arduino.cc/arduino-mkr1000-wifi), [`MKR WiFi 1010`](https://store.arduino.cc/arduino-mkr-wifi-1010), [`Nano 33 IoT`](https://store.arduino.cc/arduino-nano-33-iot), [`MKR GSM 1400`](https://store.arduino.cc/arduino-mkr-gsm-1400-1415), [`MKR NB 1500`](https://store.arduino.cc/arduino-mkr-nb-1500-1413), [`MKR WAN 1300/1310`](https://store.arduino.cc/mkr-wan-1310) :heavy_check_mark:
* [arduino-pico](https://github.com/earlephilhower/arduino-pico): [`Raspberry Pi Pico`](https://www.raspberrypi.org/products/raspberry-pi-pico), `Adafruit Feather RP2040`, ... :heavy_check_mark:
