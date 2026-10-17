# STM32 BME280 Sensor-to-CAN Transmitter

## Overview

This project implements the STM32 firmware component of an Embedded Linux C++ Sensor Monitoring System. The firmware reads environmental measurements from a BME280 sensor over I2C and transmits the data over a CAN bus to a BeagleBone Black running Linux.

The STM32 acts as the sensor interface and CAN transmitter, while the BeagleBone Black receives and processes the transmitted frames using Linux SocketCAN and C++.

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
         | SocketCAN
         v
    C++ Sensor Receiver
         |
         +----> Timestamped Terminal Output
         |
         +----> CSV Data Logging

## Features

- Reads temperature, pressure, and humidity measurements from the BME280 sensor.
- Uses I2C communication to interface with the sensor.
- Encodes sensor measurements into an 8-byte CAN data frame.
- Transmits sensor data using CAN identifier `0x123`.
- Integrates with a Linux-based C++ receiver.
- Supports sensor measurement decoding, timestamped output, and CSV logging on the BeagleBone Black.

## Hardware Requirements

- STM32 development board
- BME280 environmental sensor module
- Compatible CAN transceiver
- BeagleBone Black
- Compatible CAN interface for the BeagleBone Black
- CAN bus wiring and termination resistors
- Suitable USB cables and power supplies

**Hardware note:** The STM32 CAN peripheral must connect to the physical CAN bus through a compatible CAN transceiver. Do not connect CANH and CANL directly to STM32 microcontroller pins.

## Communication Protocol

The STM32 transmits a standard CAN data frame with the following format:

| Parameter | Description |
|---|---|
| CAN identifier | `0x123` |
| Frame type | Standard CAN data frame |
| Data length | 8 bytes |
| Bytes 0-1 | Signed temperature value, scaled by 100 |
| Bytes 2-5 | Pressure in pascals, represented as a 32-bit value |
| Bytes 6-7 | Relative humidity, scaled by 100 |

### Measurement Decoding

The Linux receiver interprets the payload as follows:

- **Temperature:** Signed 16-bit integer divided by 100 to obtain degrees Celsius.
- **Pressure:** 32-bit pressure value in pascals divided by 100 to obtain hPa.
- **Humidity:** Unsigned 16-bit integer divided by 100 to obtain %RH.

The STM32 firmware and Linux receiver must use the same byte order, signedness, scaling, and pressure units.

## Development Environment

- STM32CubeIDE
- STM32CubeMX, if used
- STM32 HAL drivers, if used
- STM32 C/C++ toolchain

## Build and Flash

1. Open the STM32 firmware project in STM32CubeIDE.
2. Verify the target microcontroller and clock configuration.
3. Configure the I2C peripheral for BME280 communication.
4. Configure the STM32 CAN peripheral and compatible CAN transceiver.
5. Verify that the CAN bit rate and frame format match the receiving system.
6. Build the firmware.
7. Flash the firmware onto the STM32 board.
8. Connect the STM32 transmitter to the CAN bus.

## Testing and Validation

- Confirm that the BME280 sensor returns valid environmental measurements.
- Verify temperature, pressure, and humidity encoding.
- Confirm that CAN frames use identifier `0x123` and an 8-byte payload.
- Check that CAN settings match the receiver configuration.
- Verify that the BeagleBone Black receives and decodes sensor measurements.
- Confirm that the Linux receiver displays timestamped readings and records measurements in a CSV file.

## Integration with the Linux Receiver

The STM32 firmware handles sensor acquisition and CAN transmission. The BeagleBone Black receives CAN frames through Linux SocketCAN and processes the data using C++.

The Linux receiver decodes incoming frames, displays timestamped sensor measurements, and stores readings in a CSV log for further analysis.

## Project Context

The **Embedded Linux C++ Sensor Monitoring System** demonstrates integration between an STM32 microcontroller and an embedded Linux platform through CAN communication. It combines sensor interfacing, data encoding, communication protocols, and Linux-based sensor data logging.
