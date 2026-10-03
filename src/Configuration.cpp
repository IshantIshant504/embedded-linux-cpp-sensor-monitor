#include "Configuration.hpp"
#include <fstream>
#include <string>
#include <stdexcept>

Configuration::Configuration(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
    {
        throw std::runtime_error("Could not open configuration file");
    }

    std::string line;

    if (!std::getline(file, line))
    {
        throw std::runtime_error("Configuration file is empty");
    }

    std::size_t position = line.find('=');

    if (position == std::string::npos)
    {
        throw std::runtime_error("Invalid configuration format");
    }

    try
    {
        temperature = std::stof(line.substr(position + 1));
    }
    catch (...)
    {
        throw std::runtime_error("Invalid temperature value");
    }

    if (temperature < -50.0f || temperature > 100.0f)
    {
        throw std::runtime_error("Temperature value is out of range");
    }
}

float Configuration::getTemperature() const
{
    return temperature;
}
