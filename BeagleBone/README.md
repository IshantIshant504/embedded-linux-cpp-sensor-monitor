
# BME280 Embedded Linux CAN Sensor Monitor

## Overview

An Embedded Linux C++ application that receives BME280 sensor measurements over CAN, decodes the payload, displays timestamped readings, and records the measurements in a CSV file.

## Architecture

BME280 Sensor → STM32 Transmitter → CAN Bus → BeagleBone Black → C++ Receiver → CSV Log

## Features

- Linux SocketCAN communication
- CAN frame filtering and validation
- Temperature, pressure, and humidity decoding
- Timestamped console output
- CSV data logging
- Graceful shutdown handling
- Automated decoder tests

## Technologies

- C++
- Embedded Linux / Debian
- Linux SocketCAN
- STM32
- BeagleBone Black
- CAN bus
- CSV logging

## Build

Compile the receiver:

```bash
g++ -std=c++17 -Wall -Wextra \
    bme280_can_receiver.cpp sensor_decoder.cpp \
    -o bme280_can_receiver
```

Compile the tests:

```bash
g++ -std=c++17 -Wall -Wextra \
    test_sensor_decoder.cpp sensor_decoder.cpp \
    -o test_sensor_decoder
```

Run the tests:

```bash
./test_sensor_decoder
```

## Run

Ensure `can0` is configured and active, then run:

```bash
sudo ./bme280_can_receiver
```

Press Ctrl+C to stop.

## Data Format

The CSV file uses these columns:

`Timestamp,Temperature_C,Pressure_hPa,Humidity_RH`

## Current Test Results

Nine decoder tests pass, covering valid frames, sensor-value decoding, invalid CAN IDs, incorrect DLC, extended frames, remote frames, and error frames.

## Future Improvements

- CMake build system
- Additional integration tests
- Improved signal-safe shutdown
- Sensor-data visualization

