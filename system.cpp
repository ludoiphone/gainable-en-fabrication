#include "system.h"

void System::productions()
{
	if (contactThermostat.GetThermostats() == true)
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
