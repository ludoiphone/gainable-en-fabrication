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
    
    Modes modes;
    
    while (1) 
    {
        time ( & rawtime);
        printf("Date : %s\n\n", ctime ( & rawtime));
        
        if (time(nullptr) - chrono >= deltaT)
        {
            modes.SetTemperatureExt(sondeExt.GetTempExt());
            chrono = time(nullptr);
        }
        
        modes.productions();
    }
    return 0;
}
