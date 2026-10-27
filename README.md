# Embedded Linux C++ Sensor Monitoring System

## Overview

This project demonstrates an embedded sensor monitoring system integrating an STM32 microcontroller, a BeagleBone Black running Linux, and an Ubuntu-based TCP receiver.

A BME280 sensor measures temperature, pressure, and relative humidity. The STM32 reads sensor measurements over I2C and transmits the data over a CAN bus. The BeagleBone Black receives the CAN frames through Linux SocketCAN and processes them using C++.

The BeagleBone application decodes the sensor data, displays measurements on an OLED, records readings in a CSV file, and forwards the data to an Ubuntu system over TCP.

## System Architecture

```text
BME280 Sensor
     |
     | I2C
     v
STM32 Nucleo-G431RB
     |
     | CAN Frame (ID: 0x123)
     v
CAN Bus
     |
     v
BeagleBone Black
     |
     | Linux SocketCAN
     v
C++ CAN Receiver
     |
     +----> Sensor Data Decoder
     |
     +----> Timestamped Terminal Output
     |
     +----> CSV Data Logging
     |
     +----> SSD1306 OLED Display
     |
     | TCP/IP
     v
Ubuntu Linux
     |
     v
C++ TCP Receiver
```

## Key Features

- BME280 environmental sensor integration.
- I2C communication between the sensor and STM32.
- CAN communication between the STM32 and BeagleBone Black.
- Linux SocketCAN-based CAN frame reception.
- C++ sensor data decoding.
- Timestamped temperature, pressure, and humidity output.
- CSV-based sensor measurement logging.
- SSD1306 OLED display integration.
- TCP communication between the BeagleBone Black and Ubuntu.

## Hardware Requirements

- STM32 Nucleo-G431RB development board.
- BME280 environmental sensor module.
- Compatible CAN transceiver for the STM32.
- BeagleBone Black Rev C.
- Compatible CAN interface for the BeagleBone Black.
- SSD1306 OLED display, 128 × 32 pixels.
- CAN bus wiring and termination resistors.
- Suitable USB cables and power supplies.

**Hardware note:** Use compatible CAN transceivers and matching CAN communication settings. Do not connect CANH and CANL directly to STM32 microcontroller logic pins.

## Software and Technologies

| Technology | Purpose |
|---|---|
| Embedded C/C++ | STM32 firmware and Linux application development |
| STM32CubeIDE | STM32 firmware development |
| I2C | BME280 and OLED communication |
| CAN | Communication between embedded devices |
| Linux / Debian | BeagleBone operating system |
| SocketCAN | Linux CAN interface |
| C++ | Sensor decoding and Linux applications |
| CMake | BeagleBone application build configuration |
| TCP/IP sockets | Sensor-data transmission to Ubuntu |
| CSV | Sensor data logging |
| Git and GitHub | Version control and project documentation |

## CAN Communication Protocol

The STM32 transmits a standard CAN data frame using identifier `0x123` and an 8-byte payload.

| Payload Bytes | Measurement | Encoding |
|---|---|---|
| 0–1 | Temperature | Signed 16-bit integer, scaled by 100 |
| 2–5 | Pressure | 32-bit value in pascals |
| 6–7 | Relative humidity | Unsigned 16-bit integer, scaled by 100 |

The Linux receiver decodes the measurements into temperature in degrees Celsius, pressure in hectopascals, and relative humidity in percent.

The transmitter and receiver must use matching byte order, signedness, scaling, and pressure units.

## Repository Structure

```text
embedded-linux-cpp-sensor-monitor/
|
+-- README.md
|
+-- STM32/
|   +-- README.md
|   +-- Core/
|   +-- Drivers/
|   +-- BME280.ioc
|   +-- STM32G431RBTX_FLASH.ld
|
+-- BeagleBone/
|   +-- README.md
|   +-- CMakeLists.txt
|   +-- bme280_can_receiver.cpp
|   +-- sensor_decoder.cpp
|   +-- sensor_decoder.hpp
|   +-- oled_display.cpp
|   +-- oled_display.hpp
|
+-- Ubuntu/
    +-- README.md
    +-- sensor_tcp_receiver.cpp
```

The structure above describes the intended organization. Adjust directory names to match the actual repository.

## Build and Usage

The STM32 firmware is built and flashed using STM32CubeIDE.

The BeagleBone application is built using CMake and a compatible C++ compiler. It requires the configured CAN interface and OLED hardware.

The Ubuntu TCP receiver listens for sensor data on port `5000`. Start the TCP receiver before running the BeagleBone application.

See the STM32, BeagleBone, and Ubuntu README files for component-specific instructions.

## Project Goals

This project demonstrates practical experience with:

- Microcontroller firmware development.
- Environmental sensor integration.
- I2C and CAN communication.
- Embedded Linux application development.
- Linux SocketCAN.
- C++ programming and sensor-data decoding.
- OLED display integration.
- TCP/IP networking.
- CSV data logging.
- CMake-based builds.

## Future Improvements

- Improve TCP reconnection and error handling.
- Add configurable network settings.
- Improve structured logging and diagnostics.
- Add sensor-data visualization.
- Expand automated validation and integration testing.
