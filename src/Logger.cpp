#include "Logger.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdexcept>

Logger::Logger(const std::string& filename)
    : filename(filename)
{
}

void Logger::logTemperature(float temperature)
{
    std::ofstream file(filename, std::ios::app);

    if (!file)
    {
        throw std::runtime_error("Could not open log file");
    }

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    file << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S")
         << " | Temperature: "
         << temperature
         << " degC"
         << std::endl;
}
