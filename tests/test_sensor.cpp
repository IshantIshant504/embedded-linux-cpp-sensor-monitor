#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Configuration.hpp"
#include "SimulatedSensor.hpp"

int main()
{
    // Test 1: Valid configuration
    Configuration config("../config/sensor.conf");

    float configuredTemperature = config.getTemperature();

    assert(configuredTemperature == 32.5f);

    SimulatedSensor sensor(configuredTemperature);

    float sensorTemperature = sensor.readTemperature();

    assert(sensorTemperature == 32.5f);


    // Test 2: Missing configuration file
    bool missingFileError = false;

    try
    {
        Configuration invalidConfig("../config/missing.conf");
    }
    catch (const std::exception&)
    {
        missingFileError = true;
    }

    assert(missingFileError);


    // Test 3: Invalid temperature value
    std::ofstream invalidFile("../config/invalid_test.conf");

    invalidFile << "temperature = abc\n";

    invalidFile.close();

    bool invalidTemperatureError = false;

    try
    {
        Configuration invalidConfig("../config/invalid_test.conf");
    }
    catch (const std::exception&)
    {
        invalidTemperatureError = true;
    }

    assert(invalidTemperatureError);


    std::cout << "All sensor tests passed." << std::endl;

    return 0;
}
