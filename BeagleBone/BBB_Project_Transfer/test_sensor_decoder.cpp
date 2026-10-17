#include "sensor_decoder.hpp"
#include <iostream>
#include <cstdint>
#include <cstring>
#include <linux/can.h>


int main()
{
    int passed = 0;
    int failed = 0;

    auto check = [&](const char* name, bool condition) {
        if (condition) {
            std::cout << "[PASS] " << name << '\n';
            ++passed;
        } else {
            std::cout << "[FAIL] " << name << '\n';
            ++failed;
        }
    };

    struct can_frame frame {};
    frame.can_id = 0x123;
    frame.can_dlc = 8;

    // Temperature = 25.00 C
    frame.data[0] = 0x09;
    frame.data[1] = 0xC4;

    // Pressure = 96000 Pa = 960.00 hPa
    frame.data[2] = 0x00;
    frame.data[3] = 0x01;
    frame.data[4] = 0x77;
    frame.data[5] = 0x00;

    // Humidity = 50.00 %RH
    frame.data[6] = 0x13;
    frame.data[7] = 0x88;

    SensorData sensor{};

    bool valid = decode_sensor_frame(frame, sensor);

    check("Valid frame accepted", valid);

    check("Temperature decoded correctly",
          valid && sensor.temperature_c == 25.00);

    check("Pressure decoded correctly",
          valid && sensor.pressure_hpa == 960.00);

    check("Humidity decoded correctly",
          valid && sensor.humidity_rh == 50.00);

    struct can_frame invalid = frame;
    invalid.can_id = 0x124;
    check("Incorrect CAN ID rejected",
          !decode_sensor_frame(invalid, sensor));

    invalid = frame;
    invalid.can_dlc = 7;
    check("Incorrect DLC rejected",
          !decode_sensor_frame(invalid, sensor));

    invalid = frame;
    invalid.can_id = 0x123 | CAN_EFF_FLAG;
    check("Extended CAN frame rejected",
          !decode_sensor_frame(invalid, sensor));

    invalid = frame;
    invalid.can_id = 0x123 | CAN_RTR_FLAG;
    check("Remote frame rejected",
          !decode_sensor_frame(invalid, sensor));

    invalid = frame;
    invalid.can_id = CAN_ERR_FLAG;
    check("Error frame rejected",
          !decode_sensor_frame(invalid, sensor));

    std::cout << "\nPassed: " << passed
              << " | Failed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
