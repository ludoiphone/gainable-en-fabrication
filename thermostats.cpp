#include "thermostats.h"

#include "gpioPin.hpp"

#define OFF HIGH
#define ON LOW

const int pinThermostat {17};

Thermostat::Thermostat()
{
	pinMode (pinThermostat, INPUT_PULLUP);
}

bool Thermostat::GetContactThermostat()
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
