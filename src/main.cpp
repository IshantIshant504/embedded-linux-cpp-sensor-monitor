
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include "Logger.hpp"
#include "Configuration.hpp"
#include "SimulatedSensor.hpp"

int main()
{
    std::cout << "Embedded Linux Sensor Monitor" << std::endl;
    try
{
    Configuration config("../config/sensor.conf");

    SimulatedSensor sensor(config.getTemperature());
    Logger logger("../logs/sensor.log");

    float sharedTemperature = sensor.readTemperature();

    std::mutex temperatureMutex;

    auto sensorTask = [&]()
    {
        for (int i = 0; i < 5; i++)
        {
            float newTemperature = sensor.readTemperature();

            {
                std::lock_guard<std::mutex> lock(temperatureMutex);
                sharedTemperature = newTemperature;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    auto loggerTask = [&]()
    {
        for (int i = 0; i < 5; i++)
        {
            float temperature;

            {
                std::lock_guard<std::mutex> lock(temperatureMutex);
                temperature = sharedTemperature;
            }

           logger.logTemperature(temperature);
         

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    std::thread sensorThread(sensorTask);
    std::thread loggerThread(loggerTask);

    sensorThread.join();
    loggerThread.join();
      }
     catch (const std::exception& error)
     {
        std::cerr << "Error:  " << error.what() << std::endl;
        return 1;
      }

    return 0;
}
