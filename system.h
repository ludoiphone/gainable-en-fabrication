#ifndef SYSTEM_H
#define SYSTEM_H

#include "thermostats.h"

class System
{
    Thermostat commandeThermostat;
    
    double temperatureExt;

    void modeChauffage();
    void modeFroid();
    
public:
    ~System();

    void SetTemperatureExt(double temperatureExtLue);
    void production();
};

#endif // SYSTEM_H
