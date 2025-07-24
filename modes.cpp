#include "modes.h"

void Modes::productions()
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

void Modes::SetTemperatureExt(double temperatureExtLue)
{
	temperatureExt = temperatureExtLue;
}

Modes::~Modes()
{	
}
