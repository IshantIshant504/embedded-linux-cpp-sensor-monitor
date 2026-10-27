# BeagleBone Black — Embedded Linux Sensor Monitoring

## 1. Project Overview

This part of the project implements the Embedded Linux receiver on the **BeagleBone Black Rev C**. An STM32 Nucleo-G431RB reads environmental measurements from a BME280 sensor and transmits them to the BeagleBone Black over the CAN bus.

On the BeagleBone Black, a C++ application receives CAN frames through Linux SocketCAN, decodes the sensor measurements, logs the readings to a CSV file, and displays the data on an I²C OLED. The application can also forward sensor readings to a C++ TCP receiver running on Ubuntu.

This project demonstrates communication between a microcontroller and a Linux-based embedded system using CAN, together with C++ application development on Embedded Linux.

## 2. System Architecture

```text
             BME280 Sensor
                   |
                   | I²C
                   v
          STM32 Nucleo-G431RB
          Embedded C / Firmware
                   |
                   | CAN TX/RX
                   v
             CAN Transceiver
                   |
                   | CANH / CANL
                   v
          BeagleBone Black Rev C
                   |
                   v
             Linux SocketCAN
                   |
                   v
          C++ CAN Receiver
                   |
          +--------+---------+
          |        |         |
          v        v         v
       Decode    CSV Log   I²C OLED
       Sensor              Display
       Data
                   |
                   v
             TCP Connection
                   |
                   v
          Ubuntu C++ Receiver
              Port 5000
```

**Data flow:** BME280 → STM32 → CAN bus → BeagleBone Black → C++ processing → CSV/OLED/TCP.

## 3. Hardware Components

- BeagleBone Black Rev C
- STM32 Nucleo-G431RB
- BME280 environmental sensor
- CAN transceiver for the STM32
- CAN interface/transceiver for the BeagleBone Black
- 120 Ω CAN termination resistors, as required by the bus topology
- SSD1306-compatible 0.91-inch, 128 × 32 OLED display
- Jumper wires and appropriate power supplies
- Ubuntu virtual machine running on a host computer for TCP reception

## 4. CAN Communication Between STM32 and BeagleBone Black

### 4.1 STM32 Transmitter

The STM32 Nucleo-G431RB reads temperature, pressure, and humidity measurements from the BME280 sensor over I²C. The firmware encodes these measurements into a CAN frame and transmits it to the BeagleBone Black.

The STM32 firmware is documented separately in [`STM32/README.md`](../STM32/README.md).

### 4.2 CAN Bus Wiring

The CAN bus connects the STM32-side transceiver to the BeagleBone-side CAN interface.

| Signal | Connection |
|---|---|
| STM32 CAN TX | STM32-side transceiver TXD |
| STM32 CAN RX | STM32-side transceiver RXD |
| CANH | CANH on the other CAN interface |
| CANL | CANL on the other CAN interface |
| Ground | Common ground where required by the interfaces |

**Important:** TXD and RXD are logic-level signals between the STM32 and its CAN transceiver. CANH and CANL are the differential bus signals. They are not interchangeable.

The CAN bus must use compatible transceivers and appropriate termination at the two physical ends of the bus.

### 4.3 CAN Configuration

The Linux CAN interface used by the receiver is `can0`, configured at **125 kbit/s**.

Bring the interface down before configuring it:

```bash
sudo ip link set can0 down
```

Set the CAN bitrate:

```bash
sudo ip link set can0 type can bitrate 125000
```

Bring the interface up:

```bash
sudo ip link set can0 up
```

Verify the configuration:

```bash
ip -details link show can0
```

These commands assume that the CAN hardware and driver have already created the `can0` interface.

### 4.4 CAN Frame Format

The STM32 transmits the sensor readings using standard CAN identifier `0x123` and an 8-byte payload.

| Payload bytes | Data | Encoding | Decoding |
|---|---|---|---|
| 0–1 | Temperature | Signed integer, temperature × 100 | Divide by 100 |
| 2–5 | Pressure | Unsigned integer in pascals | Divide by 100 to obtain hPa |
| 6–7 | Humidity | Unsigned integer, humidity × 100 | Divide by 100 |

The receiver must interpret the fields using the same byte order and signedness as the STM32 firmware.

For example, a temperature encoded as `2345` represents `23.45 °C`. A pressure value of `101325` represents `1013.25 hPa`, and a humidity value encoded as `4567` represents `45.67%`.

### 4.5 Receiving CAN Frames

The BeagleBone Black uses **Linux SocketCAN** to access the CAN interface. The C++ application receives CAN frames, checks the message identifier, and extracts the sensor measurements from the payload.

You can inspect incoming CAN traffic from a terminal using:

```bash
candump can0
```

If `candump` is not installed, install the appropriate CAN utilities package for your Linux distribution.

## 5. Embedded Linux C++ Application

The main application receives and processes the sensor data on the BeagleBone Black.

### Main Functions

- Receive CAN frames through SocketCAN.
- Process frames using CAN identifier `0x123`.
- Decode temperature, pressure, and humidity.
- Record sensor readings in CSV format.
- Display sensor readings on the OLED.
- Forward readings to an Ubuntu receiver over TCP when the TCP connection is configured.

### Source Code Structure

| File | Purpose |
|---|---|
| `bme280_can_receiver.cpp` | Main application for receiving and processing CAN sensor data |
| `sensor_decoder.cpp` | Sensor payload decoding implementation |
| `sensor_decoder.hpp` | Sensor decoder declarations |
| `oled_display.cpp` | OLED display implementation |
| `oled_display.hpp` | OLED display declarations |
| `CMakeLists.txt` | CMake build configuration |
| `test_sensor_decoder.cpp` | Sensor decoder test source |
| `oled_header_test.cpp` | OLED header/interface test source |
| `oled_i2c_test.cpp` | OLED I²C test source |

The test programs are separate from the main receiver application and should be built or run according to their respective source files and build configuration.

## 6. OLED Display Integration

The project uses an SSD1306-compatible **128 × 32 OLED display** connected to the BeagleBone Black over I²C.

### Wiring

| OLED Pin | BeagleBone Black Pin |
|---|---|
| VCC | P9_3 — 3.3 V |
| GND | P9_1 — Ground |
| SDA | P9_18 — I²C data |
| SCL | P9_17 — I²C clock |

The OLED uses I²C address `0x3C`.

**Note:** P9_19 and P9_20 are used for the CAN connection in this project. The OLED uses P9_17 and P9_18, so the existing CAN wiring should remain unchanged.

The OLED implementation is separated into `oled_display.cpp` and `oled_display.hpp`.

## 7. CSV Data Logging

The receiver can record decoded sensor readings in CSV format for later analysis.

A typical CSV structure could contain:

```csv
timestamp,temperature_c,pressure_hpa,humidity_percent
2026-10-27 12:00:00,23.45,1013.25,45.67
```

The example above illustrates the data format; it is not a claim that these are actual measurements collected by the project.

CSV logging makes it possible to inspect recorded measurements using spreadsheet software or analyze them with Python.

## 8. TCP Communication with Ubuntu

In addition to local processing on the BeagleBone Black, the project supports forwarding sensor data to a C++ TCP receiver running on Ubuntu in a VirtualBox virtual machine.

The intended data flow is:

```text
STM32
  |
  | CAN
  v
BeagleBone Black
  |
  | C++ TCP client
  | TCP port 5000
  v
Ubuntu Virtual Machine
  |
  v
C++ TCP Receiver
```

The Ubuntu-side receiver is implemented in `sensor_tcp_receiver.cpp`.

For successful communication:

1. Ensure the Ubuntu TCP receiver is configured to listen on port `5000`.
2. Determine an IP address that the BeagleBone Black can reach.
3. Ensure the BeagleBone Black has network connectivity to that address.
4. Allow TCP port `5000` through the relevant firewall, if applicable.
5. Start the Ubuntu receiver before testing the TCP connection from the BeagleBone Black.

The correct IP address depends on the network configuration of the BeagleBone Black, Windows host, and VirtualBox virtual machine. Do not assume that the VM's IP address is reachable from the BeagleBone Black without checking the network setup.

## 9. Build and Run

Run these commands from the BeagleBone project directory containing `CMakeLists.txt`.

### Build

Configure the project:

```bash
cmake -S . -B build
```

Compile the application:

```bash
cmake --build build
```

### Run

If the build produces an executable named `bme280_can_receiver` in the build directory, start it with:

```bash
sudo ./build/bme280_can_receiver
```

Root privileges may be required for CAN and hardware access, depending on the system configuration.

The commands above assume that the current `CMakeLists.txt` defines the main executable with that name. If the build configuration uses a different target name or does not include a source file, adjust the command or build configuration accordingly.

## 10. Troubleshooting

### CAN interface not found

Check whether Linux has created `can0`:

```bash
ip link show
```

If `can0` is missing, check the CAN interface hardware, driver, device-tree configuration, and system setup.

### No CAN frames received

Check the following:

- Both CAN interfaces use the same bitrate.
- CANH and CANL are connected correctly.
- The CAN transceivers are compatible with their respective interfaces.
- The bus is terminated appropriately.
- The STM32 firmware is transmitting frames with identifier `0x123`.
- The Linux CAN interface is up.

Use `candump can0` to check whether frames reach the BeagleBone Black.

### OLED not displaying data

Check the OLED power supply, ground, SDA/SCL wiring, I²C configuration, and device address `0x3C`. Also verify that the OLED driver and display initialization code are included in the build.

### TCP receiver not getting data

Confirm that the Ubuntu receiver is running, the IP address is reachable from the BeagleBone Black, and port `5000` is accessible. VirtualBox network mode and firewall rules can affect connectivity.

## 11. Future Improvements

Potential extensions include:

- More robust error handling and reconnection for CAN and TCP.
- Improved CSV logging with configurable file paths and timestamps.
- Additional automated tests for sensor decoding.
- A configurable TCP destination and message format.
- Improved OLED layout and display refresh behavior.

## Related Documentation

- [Main Project README](../README.md)
- [STM32 Firmware README](../STM32/README.md)
