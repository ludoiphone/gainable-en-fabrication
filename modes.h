#ifndef MODES_H
#define MODES_H

#include "thermostats.h"
#include "froid.h"
#include "chauffage.h"

class Modes
{
    Froid modeFroid;
    
    Chauffage modeChauffage;
    
    Thermostat contactThermostat;
    
    double temperatureExt;
	
public:
    ~Modes();

    void SetTemperatureExt(double temperatureExtLue);
    void productions();
};

#endif // MODES_H
