#ifndef DS18B20_H
#define DS18B20_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

class DS18B20
{
    char buffer[10];
    char * end;
    float tempExtLue, tempUnitExtLue, tempEchExtLue, tempUnitIntLue, tempEchIntLue;
    
public:
    ~DS18B20();
    
    float GetTempExt();
    float GetTempUnitExt();
    float GetTempEchExt();
    float GetTempUnitInt();
    float GetTempEchInt();
};

#endif // DS18B20_H
