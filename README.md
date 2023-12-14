# MycilaESPConnect

[![License: MIT](https://img.shields.io/badge/License-GPL%203.0-yellow.svg)](https://opensource.org/licenses/gpl-3-0)
[![Continuous Integration](https://github.com/mathieucarbou/MycilaESPConnect/actions/workflows/ci.yml/badge.svg)](https://github.com/mathieucarbou/MycilaESPConnect/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mathieucarbou/library/MycilaESPConnect.svg)](https://registry.platformio.org/libraries/mathieucarbou/MycilaESPConnect)

-----

Simple & Easy WiFi Manager with Captive Portal for ESP32

A simplistic approach to a WiFi Manager on ESP32 MCUs. Comes with captive portal to configure modules without any hassle.

-----

> This fork is based on [https://github.com/ayushsharma82/ESPConnect](https://github.com/ayushsharma82/ESPConnect).
> I highly recommend looking at all OSS projects (and products) from [@ayushsharma82](https://github.com/ayushsharma82).
> He is making great Arduino libraries.

## Changes

- **Logo**: user is responsible to provide a logo at this path: `/logo`

- **AP Mode**: a new choice is added to the captive portal so that the user can remain in AP mode

- **Network State Machine**: a better state machine is implemented to handle switching between Captive Portal, AP Mode and STA mode

- **New API**: API has been completely rewritten

- **Callback**: Listen to Network State changes

- **Blocking and Non-blocking modes**: ESPConnect can be configured to loop and wait for the user to complete the Captive Portal steps, or the app can continue working in the background and the Captive Portal will be launched as needed.

- **Flexible Configuration:** ESPConnect can either control the configuration persistence for you or let you do it

- **mDNS / DNS Support**

See the examples and API for more !
