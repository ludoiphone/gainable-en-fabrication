#include "modes.h"

void Modes::productions()
{
	if (contactThermostat.GetThermostats() == true)
	{
		if (temperatureExt < 13.5) {
			modeChauffage.chauffage();
		} else {
			modeFroid.refroidissement();
		}
	}
} 

void Modes::SetTemperatureExt(double temperatureExtLue)
{
	temperatureExt = temperatureExtLue;
	
	cout << "temperatureExterieur : " << temperatureExt << endl;
}

Modes::~Modes()
{
	cout << "destructeur Modes" << endl;
	
}
