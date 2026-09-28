#include "Sensor.hpp"
#include <iostream>

Sensor::Sensor()
{
    temperature = 25.0f;
}

void Sensor::read()
{
     std::cout << "Tempertature: "
               << temperature
               << "  degC"
               << std::endl;
}
