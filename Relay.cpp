#include "Relay.h"

void System::production()
{
	if (commandeThermostat.GetEtatThermostats() == true)
	{
		if (temperatureExt < 13.5) {
			modeChauffage();
		} else {
			modeFroid();
		}
	}
} 

void System::SetTemperatureExt(double temperatureExtLue)
{
	temperatureExt = temperatureExtLue;
}

void System::modeChauffage()
{

}

void System::modeFroid()
{

}

System::~System()
{	
}
