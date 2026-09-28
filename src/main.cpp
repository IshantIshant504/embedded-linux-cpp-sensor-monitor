#include <iostream>
#include "Sensor.hpp"
int main()
{
      std::cout << "Embedded Linux Sensor Monitor" << std::endl;

      Sensor sensor;
      sensor.read();

      return 0;
}

