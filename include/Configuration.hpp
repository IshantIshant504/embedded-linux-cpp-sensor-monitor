#ifndef CONFIGUARATION_HPP
#define CONFIGUARATION_HPP

#include <string>

class Configuration
{
public:
    explicit Configuration(const std::string& filename);
 
    float getTemperature() const;

private:
     float temperature;
};

#endif

