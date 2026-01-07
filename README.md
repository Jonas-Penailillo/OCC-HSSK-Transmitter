# OCC HSSK Transmitter Firmware

This repository contains the firmware implementation for an Optical Camera Communication (OCC) transmitter using an ESP32 microcontroller and dual WS2812B LED matrices. This work is part of the PhD Thesis by Jonas Elias Peñailillo.

## System Overview
The system implements a **Hue-Saturation Shift Keying (HSSK)** modulation scheme. It utilizes a split-grid structure (two 4x4 matrices) to transmit data encoded via Gray mapping into color symbols, effectively acting as a unified 16-LED transmitter or supporting MIMO configurations.

## Hardware Configuration
The setup consists of two 4x4 WS2812B matrices driven in parallel by an ESP32:

* **Microcontroller:** ESP32
* **LED Modules:** 2x WS2812B Matrix (4x4 LEDs each).
* **Connection:**
  * **Matrix 1 Data:** GPIO 14
  * **Matrix 2 Data:** GPIO 13
* **Power:** External 5V power supply (common ground with ESP32).

## Dependencies
To compile this project, you need the Arduino IDE with the ESP32 board support and the following library:
* [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)

## Usage
1.  Connect the LED matrices to the defined GPIO pins.
2.  Upload the `.ino` file to the ESP32.
3.  The system performs a startup sequence (Sync Signal -> Constellation -> ID) and loops the data transmission.
