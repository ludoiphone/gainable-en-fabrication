#include "Thermostats.h"

#define OFF HIGH
#define ON LOW

Thermostat::Thermostat()
{
	wiringPiSetupGpio();
	pinMode(pinThermostat, INPUT);
	pullUpDnControl(pinThermostat, PUD_UP);
}

bool Thermostat::GetContactThermostat()
{
	if (digitalRead(pinThermostat) == ON)
	{
		return true;
	}
	return false;
}

Thermostat::~Thermostat()
{
	
}
