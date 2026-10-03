#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

class Logger
{
public:
    explicit Logger(const std::string& filename);

    void logTemperature(float temperature);

private:
    std::string filename;
};

#endif
