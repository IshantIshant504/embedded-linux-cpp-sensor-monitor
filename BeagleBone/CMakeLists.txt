
cmake_minimum_required(VERSION 3.16)

project(BME280_CAN_Monitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sensor_decoder sensor_decoder.cpp)

target_compile_options(sensor_decoder PRIVATE
    -Wall -Wextra
)

add_executable(bme280_can_receiver
    bme280_can_receiver.cpp
)

target_link_libraries(bme280_can_receiver
    PRIVATE sensor_decoder
)

target_compile_options(bme280_can_receiver PRIVATE
    -Wall -Wextra
)

include(CTest)

if(BUILD_TESTING)
    add_executable(test_sensor_decoder
        test_sensor_decoder.cpp
    )

    target_link_libraries(test_sensor_decoder
        PRIVATE sensor_decoder
    )

    target_compile_options(test_sensor_decoder PRIVATE
        -Wall -Wextra
    )

    add_test(
        NAME SensorDecoderTests
        COMMAND test_sensor_decoder
    )
endif()
