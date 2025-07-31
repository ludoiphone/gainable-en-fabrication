#include "system.h"

void System::production()
{
	if (commandeThermostat.GetEtatThermostats() == true)
	{
		if (temperatureExt < 13.5) {
			modeChauffage.fonctionChauffage();
		} else {
			modeFroid.fonctionFroid();
		}
	}
} 

void System::SetTemperatureExt(double temperatureExtLue)
{
	temperatureExt = temperatureExtLue;
}

System::~System()
{	
}
