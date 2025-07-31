#include <ctime>
#include <unistd.h>

#include "ds18b20.h"
#include "system.h"

time_t rawtime;

const int deltaT {5};
unsigned long long chrono;

int main(int argc, char *argv[])
{
    DS18B20 sondeExt;
    
    System mode;
    
    while (1) 
    {
        time ( & rawtime);
        printf("Date : %s\n\n", ctime ( & rawtime));
        
        if (time(nullptr) - chrono >= deltaT)
        {
            mode.SetTemperatureExt(sondeExt.GetTempExt());
            chrono = time(nullptr);
        }
        
        mode.productions();
    }
    return 0;
}
