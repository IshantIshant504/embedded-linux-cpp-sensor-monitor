#include "SimulatedSensor.hpp"

SimulatedSensor::SimulatedSensor(float temperature)
      : temperature(temperature)
{
}

float SimulatedSensor::readTemperature()
{
   return temperature;
}
