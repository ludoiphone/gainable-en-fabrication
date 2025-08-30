#ifndef THERMOSTATS_H
#define THERMOSTATS_H

#include <wiringPi.h>

class Thermostat
{
    const int pinThermostat {17};

public:
    Thermostat();
    ~Thermostat();
    
    bool GetContactThermostat( );
};

#endif // THERMOSTATS_H
