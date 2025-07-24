#include <ctime>
#include <unistd.h>

#include "ds18b20.h"
#include "modes.h"

time_t rawtime;

const int deltaT = 5;
unsigned long long chrono;

int main(int argc, char *argv[])
{
    DS18B20 sondeExt;
    DS18B20 sondeUnitExt;
    DS18B20 sondeEchangeurExt;
    DS18B20 sondeUnitInt;
    DS18B20 sondeEchangeurInt;
    
    Modes modes;
    
    while (1) 
    {
        time ( & rawtime);
        printf("Date : %s\n\n", ctime ( & rawtime));
        
        if (time(nullptr) - chrono >= deltaT)
        {
            modes.SetTemperatures(sondeExt.GetTempExt(), sondeUnitExt.GetTempUnitExt(), sondeEchangeurExt.GetTempEchExt(),
            sondeUnitInt.GetTempUnitInt(), sondeEchangeurInt.GetTempEchInt());
            chrono = time(nullptr);
        }
        
        modes.productions();
    }
    return 0;
}
