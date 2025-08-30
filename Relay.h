#ifndef RELAY_H
#define RELAY_H

#include <wiringPi.h>
#include "Trace.h"

class Relay
{
    const int pinRelayCompresseur {5};
    const int pinRelayV4V {6};
    const int pinRelayVentilateurExterieur {13};
    const int pinRelayVitessesExterieur {16};
    const int pinRelayVentilateurInterieur {19};
    const int pinRelayVitessesInterieur {20};
    const int pinRelayEteHiver {26};
    
public:
    Relay();
    
    void highRelayEteHiver();
    void highRelayV4V();
    void highRelayCompresseur();
    void highRelayVentilationInterieur();
    void highRelayGrandeVitesseInterieur();
    void highRelayVentilationExterieur();
    void highRelayGrandeVitesseExterieur();
    
    void lowRelayEteHiver();
    void lowRelayV4V();
    void lowRelayCompresseur();
    void lowRelayVentilationInterieur();
    void lowRelayPetiteVitesseInterieur();
    void lowRelayVentilationExterieur();
    void lowRelayPetiteVitesseExterieur();
    
    void lowAllRelay();
    
    void testRelay();
};

#endif
