#include "thermostats.h"

#define OFF HIGH
#define ON LOW

const int pinThermostat {17};

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
