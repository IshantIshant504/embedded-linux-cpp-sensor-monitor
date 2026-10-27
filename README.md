# BME280 Embedded Linux CAN Sensor Monitor

## Overview

This project implements the Embedded Linux component of a sensor monitoring system. A C++ application running on the BeagleBone Black receives BME280 sensor measurements from an STM32 microcontroller over CAN.

The application uses Linux SocketCAN to receive and filter CAN frames, decodes the sensor measurements, displays timestamped readings in the terminal, records measurements in a CSV file, and displays the current readings on an SSD1306 OLED.

The application also forwards sensor data to an Ubuntu system through a TCP connection.

## Architecture

```text
STM32 + BME280
      |
      | CAN (ID: 0x123)
      v
BeagleBone Black
      |
      v
Linux SocketCAN
      |
      v
C++ CAN Receiver
      |
      +----> Sensor Data Decoder
      |
      +----> Timestamped Terminal Output
      |
      +----> CSV Logging
      |
      +----> SSD1306 OLED
      |
      | TCP/IP
      v
Ubuntu TCP Receiver
```

## Features

- Linux SocketCAN communication.
- CAN frame filtering and validation.
- Temperature, pressure, and humidity decoding.
- Timestamped terminal output.
- CSV sensor-data logging.
- SSD1306 OLED integration.
- TCP transmission to Ubuntu.
- Graceful shutdown handling.

## Technologies

- C++
- Embedded Linux / Debian
- Linux SocketCAN
- Linux I2C interface
- TCP/IP sockets
- STM32
- BeagleBone Black
- CAN bus
- SSD1306 OLED
- CMake
- CSV logging

## Source Files

| File | Purpose |
|---|---|
| `bme280_can_receiver.cpp` | Receives CAN data, logs measurements, updates the OLED, and transmits data over TCP. |
| `sensor_decoder.cpp` | Decodes sensor values from CAN frames. |
| `sensor_decoder.hpp` | Declares sensor-decoding functions and data structures. |
| `oled_display.cpp` | Implements OLED communication and display functions. |
| `oled_display.hpp` | Declares the OLED interface used by the receiver. |
| `CMakeLists.txt` | Configures the C++ build. |

## Hardware

- BeagleBone Black Rev C.
- CAN interface connected to the STM32 CAN bus.
- SSD1306 OLED display, 128 × 32 pixels.
- CAN bus termination resistors.
- Suitable power supplies and wiring.

### OLED Connections

| OLED Pin | BeagleBone Black Pin |
|---|---|
| VCC | P9_3 (3.3 V) |
| GND | P9_1 |
| SDA | P9_18 |
| SCL | P9_17 |

The OLED uses the I2C interface exposed through `/dev/i2c-1`. The CAN connections on P9_19 and P9_20 must remain unchanged.

## CAN Configuration

The application uses the Linux CAN interface `can0` and filters for CAN identifier `0x123`.

The configured bitrate is 125000 bits per second.

Example configuration:

```bash
sudo ip link set can0 down
sudo ip link set can0 type can bitrate 125000 restart-ms 1000
sudo ip link set can0 up
```

Ensure that the CAN interface and physical bus are configured correctly before starting the application.

## Build

Install the required build tools if necessary:

```bash
sudo apt update
sudo apt install build-essential cmake
```

Navigate to the project directory:

```bash
cd ~/bme280-can-project
```

Configure the project:

```bash
cmake -S . -B build
```

Build the receiver:

```bash
cmake --build build
```

The build configuration must include the following source files:

- `bme280_can_receiver.cpp`
- `sensor_decoder.cpp`
- `oled_display.cpp`

## Run

Start the Ubuntu TCP receiver first, then configure the BeagleBone CAN interface.

Run the BeagleBone application:

```bash
sudo ./build/bme280_can_receiver
```

Press `Ctrl+C` to stop the application.

## TCP Communication

The BeagleBone Black acts as the TCP client. The Ubuntu system acts as the TCP server.

The TCP client functionality is implemented in `bme280_can_receiver.cpp`.

The application connects to the configured Ubuntu IP address on port `5000`.

The Ubuntu receiver source is `sensor_tcp_receiver.cpp`. It must be running and listening on the expected port before the BeagleBone attempts to transmit data.

The Ubuntu server address must match the address configured in the BeagleBone application.

## Data Logging

The receiver records decoded sensor measurements in a CSV file.

The CSV column format is:

```text
Timestamp,Temperature_C,Pressure_hPa,Humidity_RH
```

The actual filename and format should match the implementation in `bme280_can_receiver.cpp`.

## Troubleshooting

### CAN Issues

- Confirm that `can0` exists and is active.
- Verify the bitrate and CAN identifier.
- Check CAN wiring and termination.
- Review application output for socket errors.

### OLED Issues

- Check power and ground connections.
- Verify SDA and SCL wiring.
- Confirm that the I2C interface is enabled.
- Check access to `/dev/i2c-1`.

### TCP Issues

- Confirm that the Ubuntu TCP receiver is running.
- Verify the Ubuntu IP address and TCP port.
- Check network connectivity and firewall settings.
- Review connection errors in the receiver output.

### Build Issues

- Verify that all required source and header files are present.
- Ensure that CMake and a compatible C++ compiler are installed.
- Review compiler messages for missing source files or incompatible language features.

## Practical Notes and Lessons Learned

### 1. CAN Transceiver Wiring

One of the challenges during hardware setup was understanding the correct connections between the STM32 and the CAN transceiver.

- Connect the STM32 CAN TX signal to the transceiver's TXD pin.
- Connect the STM32 CAN RX signal to the transceiver's RXD pin.
- Connect CANH to CANH and CANL to CANL between the CAN devices.

**Lesson learned:** Carefully check the pin labels and module datasheets before making connections. TXD/RXD are logic-side signals, whereas CANH/CANL are the physical CAN bus connections.

### 2. STM32 Debug Output in Tera Term

During development, I struggled to display the complete loop information in Tera Term. I discovered that the following BSP COM initialization code affected the behavior of my application:

```c
if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
{
    Error_Handler();
}
```

Commenting out this code changed the application's behavior and helped me proceed with debugging.

**Lesson learned:** Understanding the board's BSP COM initialization and the serial interface used for debugging is important when troubleshooting missing or incomplete output in Tera Term.

## Future Improvements

- Improve TCP reconnection and error handling.
- Add configurable network settings.
- Improve structured logging.
- Add additional sensor support.
- Expand automated validation and integration testing.
