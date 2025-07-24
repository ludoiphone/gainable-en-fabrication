#include <ctime>
#include <unistd.h>

#include "modes.h"
#include "ds18b20.h"

time_t rawtime;

const int deltaT = 5;
unsigned long long chrono;

int main(int argc, char *argv[])
{
    Modes modes;
    DS18B20 temperatureExt;
    
    while (1) 
    {
        time ( & rawtime);
        printf("Date : %s\n\n", ctime ( & rawtime));
        
        if (time(nullptr) - chrono >= deltaT)
        {
            modes.SetTemperatureExt(temperatureExt.GetTempExt());
            chrono = time(nullptr);
        }
        
        modes.productions();
    }
    return 0;
}
