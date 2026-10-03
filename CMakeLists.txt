cmake_minimum_required(VERSION 3.16)

project(EmbeddedLinuxSensorMonitor
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sensor_monitor
    src/main.cpp
    src/SimulatedSensor.cpp
    src/Configuration.cpp
    src/Logger.cpp
)

target_include_directories(sensor_monitor
    PRIVATE
        include
)

enable_testing()

add_executable(sensor_test
    tests/test_sensor.cpp
    src/Configuration.cpp
    src/SimulatedSensor.cpp
)

target_include_directories(sensor_test
    PRIVATE
        include
)

add_test(
    NAME SensorTest
    COMMAND sensor_test
)
