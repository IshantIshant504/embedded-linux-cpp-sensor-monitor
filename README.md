# Embedded Linux C++ Sensor Monitoring System

## Project Overview

This project implements an embedded sensor monitoring system that integrates STM32 firmware, CAN communication, Embedded Linux, and C++ software. A BME280 environmental sensor measures temperature, pressure, and humidity. An STM32 Nucleo-G431RB collects the sensor data and transmits it over a CAN bus to a BeagleBone Black running Linux.

On the BeagleBone Black, a C++ application receives CAN frames through Linux SocketCAN, decodes the sensor measurements, and logs the data to a CSV file. An OLED display provides local access to the sensor readings, while TCP communication forwards the data to an Ubuntu environment running a separate C++ receiver.

The project demonstrates the integration of microcontroller firmware, communication protocols, Linux-based embedded software, peripheral interfaces, and network communication in a single system.

## Complete Project Setup

![Complete hardware setup of the STM32, BME280 sensor, CAN transceivers, BeagleBone Black, and OLED display](images/complete_project_setup.jpg)

*Complete hardware assembly showing the main components of the sensor monitoring system.*

## System Architecture

The project follows this data flow:

**BME280 → STM32 Nucleo-G431RB → CAN Bus → BeagleBone Black → C++ Processing → OLED Display and CSV Logging → TCP → Ubuntu C++ Receiver**

The system is divided into three main parts:

- **STM32 Firmware:** Acquires environmental measurements from the BME280 using I²C and transmits the encoded sensor data through CAN.
- **BeagleBone Black:** Receives CAN messages using Linux SocketCAN, decodes the payload in C++, displays the readings on the OLED, logs measurements to CSV, and forwards data over TCP.
- **Ubuntu Receiver:** Runs a separate C++ TCP receiver to accept sensor data transmitted by the BeagleBone Black.

## Key Features

- Environmental data acquisition using the BME280 sensor.
- STM32 firmware development using Embedded C.
- CAN-based communication between a microcontroller and a Linux-based embedded platform.
- CAN frame reception and sensor-data decoding using C++ and Linux SocketCAN.
- Local sensor monitoring through an I²C OLED display.
- CSV logging for recording sensor measurements.
- TCP-based sensor-data transmission to an Ubuntu environment.
- Modular C++ source code and CMake-based build configuration.

## Technologies and Tools

**Programming Languages:** Embedded C, C++

**Microcontroller and Embedded Hardware:** STM32 Nucleo-G431RB, BeagleBone Black Rev C, BME280, SSD1306-compatible OLED

**Communication Interfaces and Protocols:** I²C, CAN, Linux SocketCAN, TCP/IP

**Development Tools and Environment:** STM32CubeIDE, STM32CubeMX, Tera Term, Linux, Ubuntu, CMake, Git, GitHub

## Lessons Learned

### 1. CAN Transceiver Wiring

During hardware integration, I struggled with understanding the correct connections between the STM32 and the CAN transceiver. The important distinction is between the logic-side TXD/RXD pins and the physical CANH/CANL bus connections.

**Lesson learned:** The STM32 CAN TX signal connects to the transceiver's TXD pin, and the STM32 CAN RX signal connects to its RXD pin. The CAN bus lines must connect CANH to CANH and CANL to CANL.

### 2. STM32 Debugging with Tera Term

During development, I struggled to display the complete loop information in Tera Term while the following BSP COM initialization code was active:

```c
if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
{
    Error_Handler();
}
```

Commenting out this code changed the application's behavior and allowed me to proceed with debugging.

**Lesson learned:** Understanding the STM32 board's BSP COM initialization and the serial interface used for debugging is important when troubleshooting missing or incomplete output in Tera Term.

## Detailed Documentation

For implementation details, hardware configuration, and software instructions, see:

- **STM32 Firmware:** `STM32/README.md`
- **BeagleBone Black and Embedded Linux:** `BeagleBone/README.md`
