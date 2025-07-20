#include <iostream>
#include <ctime>
#include <unistd.h>

#include "modes.h"
#include "ds18b20.h"

using namespace std;

time_t rawtime;

const int deltaT = 5;
unsigned long long chrono;

int main(int argc, char *argv[])
{
    Modes fonctions;
    DS18B20 temperatureExt;
    
    while (1) 
    {
        time ( & rawtime);
        cout << "date : " << ctime ( & rawtime) << endl;
        
        if (time(nullptr) - chrono >= deltaT)
        {
            fonctions.SetTemperatureExt(temperatureExt.GetTempExt());
            chrono = time(nullptr);
        }
        
        fonctions.productions();
        
        sleep(1);
    }
    return 0;
}
