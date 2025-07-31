#ifndef SYSTEM_H
#define SYSTEM_H

#include "thermostats.h"
#include "froid.h"
#include "chauffage.h"

class System
{
    Froid modeFroid;
    
    Chauffage modeChauffage;
    
    Thermostat contactThermostat;
    
    double temperatureExt;
	
public:
    ~System();

    void SetTemperatureExt(double temperatureExtLue);
    void productions();
};

#endif // SYSTEM_H
