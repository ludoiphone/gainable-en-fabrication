#include "ds18b20.h"

float DS18B20::GetTempExt()
{
    FILE * temp = fopen("/sys/bus/w1/devices/28-3ce1e3804835/temperature","rt");
    int numread = fread(buffer,10,1,temp);
    fclose(temp);
    tempExtLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempExtLue;
}

float DS18B20::GetTempUnitExt()
{
    FILE * temp = fopen("/sys/bus/w1/devices/28-3ce1e3809744/temperature","rt");
    int numread = fread(buffer,10,1,temp);
    fclose(temp);
    tempUnitExtLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempUnitExtLue;
}

float DS18B20::GetTempEchExt()
{
    FILE * temp = fopen("/sys/bus/w1/devices/28-3ce1e38060ec/temperature","rt");
    int numread = fread(buffer,10,1,temp);
    fclose(temp);
    tempEchExtLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempEchExtLue;
}

float DS18B20::GetTempUnitInt()
{
    FILE * temp = fopen("/sys/bus/w1/devices/28-3ce1e3801251/temperature","rt");
    int numread = fread(buffer,10,1,temp);
    fclose(temp);
    tempUnitIntLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempUnitIntLue;
}

float DS18B20::GetTempEchInt()
{
    FILE * temp = fopen("/sys/bus/w1/devices/28-3ce1e3805e9f/temperature","rt");
    int numread = fread(buffer,10,1,temp);
    fclose(temp);
    tempEchIntLue = ("%5.2fC\n",strtol(buffer,&end,10)/1000.0);
    
    return tempEchIntLue;
}

DS18B20::~DS18B20()
{
}
