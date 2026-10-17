
#ifndef SENSOR_DECODER_HPP
#define SENSOR_DECODER_HPP

#include <linux/can.h>

struct SensorData {
    double temperature_c;
    double pressure_hpa;
    double humidity_rh;
};

bool decode_sensor_frame(const struct can_frame& frame,
                         SensorData& sensor);

#endif

