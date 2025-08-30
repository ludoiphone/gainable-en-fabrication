#include "ds18b20.h"

float DS18B20::get_tempExt()
{
    FILE * probe1 = fopen("/sys/bus/w1/devices/w1_bus_master1/28-3ce1e3804835/temperature","rt");
    int numread1 = fread(buffer,10,1,probe1);
    fclose(probe1);
    tempExtLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);

    return tempExtLue;
}

float DS18B20::get_tempUnitExt()
{
    FILE * probe2 = fopen("/sys/bus/w1/devices/w1_bus_master1/28-3ce1e3809744/temperature","rt");
    int numread2 = fread(buffer,10,1,probe2);
    fclose(probe2);
    tempUnitExtLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempUnitExtLue;
}

float DS18B20::get_tempEchExt()
{
    FILE * probe3 = fopen("/sys/bus/w1/devices/w1_bus_master1/28-3ce1e38060ec/temperature","rt");
    int numread3 = fread(buffer,10,1,probe3);
    fclose(probe3);
    tempEchExtLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempEchExtLue;
}

float DS18B20::get_tempUnitInt()
{
    FILE * probe4 = fopen("/sys/bus/w1/devices/w1_bus_master1/28-3ce1e3801251/temperature","rt");
    int numread4 = fread(buffer,10,1,probe4);
    fclose(probe4);
    tempUnitIntLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempUnitIntLue;
}

float DS18B20::get_tempEchInt()
{
    FILE * probe5 = fopen("/sys/bus/w1/devices/w1_bus_master1/28-3ce1e3805e9f/temperature","rt");
    int numread5 = fread(buffer,10,1,probe5);
    fclose(probe5);
    tempEchIntLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempEchIntLue;
}
