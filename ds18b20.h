#ifndef DS18B20_H
#define DS18B20_H

#include <cstdio>
#include <stdlib.h>
#include <strings.h>

#include "Trace.h"

class DS18B20
{

public:
    float get_tempExt();
    float get_tempUnitExt();
    float get_tempEchExt();
    float get_tempUnitInt();
    float get_tempEchInt();
    
private:
    char buffer[10];
    char * end;
    float tempExtLue, tempUnitExtLue, tempEchExtLue, tempUnitIntLue, tempEchIntLue;
};

#endif // DS18B20_H
