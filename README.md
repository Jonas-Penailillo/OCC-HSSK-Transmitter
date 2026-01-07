# OCC HSSK Transmitter Firmware

This repository contains the firmware implementation for an Optical Camera Communication (OCC) transmitter using an ESP32 microcontroller and WS2812B LED matrices. This work is part of the PhD Thesis by Jonas Elias Peñailillo.

## System Overview
The system implements a **Hue-Saturation Shift Keying (HSSK)** modulation scheme. It utilizes a 16-LED grid structure to transmit data encoded via Gray mapping into color symbols.

### Key Features
- **Modulation:** HSSK (Hue-Saturation Shift Keying) with 16-level constellation.
- **Hardware:** ESP32 and WS2812B LED strips.
- **Encoding:** Text-to-binary conversion with Gray coding for symbol mapping.
- **Synchronization:** Includes specific synchronization slots (color sequences) for frame detection.

## Hardware Configuration
The code is configured for the following pinout (based on `matriz16Cuadrada2F.ino`):

* **Microcontroller:** ESP32
* **LED Type:** WS2812B (NeoPixel)
* **Data Pin Strip 1:** GPIO 14
* **Data Pin Strip 2:** GPIO 13
* **LED Count:** 16 LEDs per strip (Configurable via `NUM_LEDS`)

## Dependencies
To compile this project, you need the Arduino IDE with the ESP32 board support and the following library:
* [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)

## Usage
1.  Connect the LED strips to the defined GPIO pins.
2.  Upload the `.ino` file to the ESP32.
3.  The system will perform a startup sequence (OFF -> Sync Signal -> Constellation Display -> Transmitter ID).
4.  It will then continuously loop, modulating the predefined text string into light signals.
