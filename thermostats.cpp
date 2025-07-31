#include "thermostats.h"

#include "gpioPin.hpp"

#define OFF HIGH
#define ON LOW

const int contactThermostat {17};

Thermostat::Thermostat()
{
	pinMode (contactThermostat, INPUT_PULLUP);
}

bool Thermostat::GetThermostat()
{
	if (digitalRead(contactThermostat) == ON)
	{
		return true;
	}
	return false;
}

Thermostat::~Thermostat()
{
	
}
