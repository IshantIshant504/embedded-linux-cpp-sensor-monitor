#ifndef SIMULATED_SENSOR_HPP
#define SIMULATED_SENSOR_HPP


#include "Sensor.hpp"

class SimulatedSensor : public Sensor
{
public:
      explicit SimulatedSensor(float temperature);
      float readTemperature() override;
private:
    float temperature;

};

#endif
