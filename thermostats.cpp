#include "thermostats.h"

#include "gpioPin.hpp"

#define OFF HIGH
#define ON LOW

const int thermostats = 17;

Thermostats::Thermostats()
{
	pinMode (thermostats, INPUT_PULLUP);
}

bool Thermostats::GetThermostats()
{
	if (digitalRead(thermostats) == ON)
	{
		return true;
	}
	return false;
}

Thermostats::~Thermostats()
{
	
}
