# Embedded Linux C++ Sensor Monitor

A C++17 sensor monitoring application developed for Linux, with a modular sensor architecture, configuration handling, logging, multithreading, synchronization, testing, and CMake-based build management.

## Features

- C++17
- Linux development environment
- Modular sensor architecture
- Simulated temperature sensor
- Configuration-based temperature input
- Multithreaded sensor and logger tasks
- Mutex-based synchronization
- Temperature validation
- File-based logging
- Automated tests with CTest
- GDB debugging
- Valgrind memory analysis
- CMake build system
- Compiler warning flags

## Project Structure

```text
embedded-linux-cpp-sensor-monitor/
├── config/
│   └── sensor.conf
├── docs/
├── include/
│   ├── Configuration.hpp
│   ├── Logger.hpp
│   └── SimulatedSensor.hpp
├── logs/                   # generated at runtime
├── src/
│   ├── Configuration.cpp
│   ├── Logger.cpp
│   ├── SimulatedSensor.cpp
│   └── main.cpp
├── tests/
│   └── test_sensor.cpp
├── CMakeLists.txt
└── README.md
