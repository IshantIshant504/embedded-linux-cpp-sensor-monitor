#ifndef SENSOR_HPP
#define SENSOR_HPP

class Sensor
{ 
public:
   virtual ~Sensor() = default;
      virtual float readTemperature() = 0;
};
#endif
