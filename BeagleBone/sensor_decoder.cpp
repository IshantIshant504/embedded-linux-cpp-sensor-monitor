
#include "sensor_decoder.hpp"

#include <cstdint>
#include <cstring>

bool decode_sensor_frame(const struct can_frame& frame,
                         SensorData& sensor)
{
    // Validate the incoming CAN frame.
    if (frame.can_dlc != 8 ||
        (frame.can_id & CAN_SFF_MASK) != 0x123 ||
        (frame.can_id & (CAN_EFF_FLAG |
                         CAN_RTR_FLAG |
                         CAN_ERR_FLAG))) {
        return false;
    }

    // Decode temperature: bytes 0 and 1.
    uint16_t temp_raw =
        (static_cast<uint16_t>(frame.data[0]) << 8) |
         frame.data[1];

    int16_t temp_signed;
    std::memcpy(&temp_signed, &temp_raw, sizeof(temp_signed));

    // Decode pressure: bytes 2 to 5.
    uint32_t pressure_pa =
        (static_cast<uint32_t>(frame.data[2]) << 24) |
        (static_cast<uint32_t>(frame.data[3]) << 16) |
        (static_cast<uint32_t>(frame.data[4]) << 8) |
         frame.data[5];

    // Decode humidity: bytes 6 and 7.
    uint16_t humidity_raw =
        (static_cast<uint16_t>(frame.data[6]) << 8) |
         frame.data[7];

    // Convert raw values into engineering units.
    sensor.temperature_c = temp_signed / 100.0;
    sensor.pressure_hpa = pressure_pa / 100.0;
    sensor.humidity_rh = humidity_raw / 100.0;

    return true;
}

