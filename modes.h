#ifndef MODES_H
#define MODES_H

#include <iostream>

#include "thermostats.h"
#include "froid.h"
#include "chauffage.h"

using namespace std;

class Modes
{
    Froid modeFroid;
    
    Chauffage modeChauffage;
    
    Thermostats contactThermostat;
    
    double temperatureExt;
	
public:
    ~Modes();

    void SetTemperatureExt(double temperatureExtLue);
    void productions();
};

#endif // MODES_H
