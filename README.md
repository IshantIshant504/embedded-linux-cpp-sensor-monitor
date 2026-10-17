# Embedded Linux C++ Sensor Monitoring System

## Overview

This project demonstrates an embedded sensor monitoring system integrating an STM32 microcontroller with a BeagleBone Black running Linux.

A BME280 sensor measures temperature, pressure, and relative humidity. The STM32 reads the sensor measurements over I2C and transmits them over a CAN bus. The BeagleBone Black receives the CAN frames using Linux SocketCAN and processes the sensor data using C++.

The receiver displays timestamped measurements in the terminal and records them in a CSV file for further analysis.

## System Architecture

    BME280 Sensor
         |
         | I2C
         v
    STM32 Microcontroller
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
         v
    Sensor Data Decoder
         |
         +----> Timestamped Terminal Output
         |
         +----> CSV Data Logging

## Key Features

- BME280 environmental sensor integration.
- I2C communication between the sensor and STM32.
- CAN communication between the STM32 and BeagleBone Black.
- Linux SocketCAN-based CAN frame reception.
- C++ sensor data decoding.
- Timestamped temperature, pressure, and humidity output.
- CSV-based sensor measurement logging.
- Unit tests for sensor frame decoding and CAN frame validation.

## Hardware Requirements

- STM32 development board.
- BME280 environmental sensor module.
- Compatible CAN transceiver for the STM32.
- BeagleBone Black.
- Compatible CAN interface for the BeagleBone Black.
- CAN bus wiring and termination resistors.
- Suitable USB cables and power supplies.

**Hardware note:** Use compatible CAN transceivers and matching CAN communication settings. Do not connect CANH and CANL directly to STM32 microcontroller logic pins.

## Software and Technologies

| Technology | Purpose |
|---|---|
| Embedded C/C++ | Firmware and application development |
| STM32CubeIDE | STM32 firmware development |
| I2C | BME280 sensor communication |
| CAN | Communication between embedded devices |
| Linux / Debian | BeagleBone operating system |
| SocketCAN | Linux CAN interface |
| C++17 | Linux receiver and sensor decoder |
| GCC / G++ | Compilation |
| CSV | Sensor data logging |
| Git and GitHub | Version control and documentation |

## CAN Communication Protocol

The STM32 transmits a standard CAN data frame using identifier `0x123` and an 8-byte payload.

| Payload Bytes | Measurement | Encoding |
|---|---|---|
| 0-1 | Temperature | Signed 16-bit integer, scaled by 100 |
| 2-5 | Pressure | 32-bit value in pascals |
| 6-7 | Relative humidity | Unsigned 16-bit integer, scaled by 100 |

The Linux receiver decodes the measurements into:

- Temperature in degrees Celsius (°C).
- Pressure in hectopascals (hPa).
- Relative humidity in percent (%RH).

The transmitter and receiver must use matching byte order, signedness, scaling, and pressure units.

## Repository Structure

    embedded-linux-cpp-sensor-monitor/
    |
    +-- README.md
    |
    +-- STM32/
    |   +-- README.md
    |   +-- Firmware source files and project configuration
    |
    +-- BeagleBone/
        +-- README.md
        +-- bme280_can_receiver.cpp
        +-- sensor_decoder.hpp
        +-- sensor_decoder.cpp
        +-- test_sensor_decoder.cpp
        +-- CMakeLists.txt
        +-- .
