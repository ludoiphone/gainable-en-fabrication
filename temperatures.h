#ifndef TEMPERATURES_H
#define TEMPERATURE_H

#include <ctime>

#include "DS18B20.H"

class Temperatures
{
	DS18B20 temperaturesDS18B20;
	
	const int deltaT = 2;
	unsigned long long chrono;
	
	double d_sondeExtLue;
	
public:
    Temperatures();
    ~Temperatures();
    
    void lectureSondeExt(double sondeExtLue);
	
};


#endif // TEMPERATURES_H
