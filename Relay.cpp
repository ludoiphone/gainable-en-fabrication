#include "Relay.h"

#define OFF HIGH
#define ON LOW

Relay::Relay()
{
    wiringPiSetupGpio();
    pinMode(pinRelayCompresseur, OUTPUT);
    digitalWrite (pinRelayCompresseur, OFF);
    pinMode(pinRelayV4V, OUTPUT);
    digitalWrite (pinRelayV4V, OFF);
    pinMode(pinRelayVentilateurExterieur, OUTPUT);
    digitalWrite(pinRelayVentilateurExterieur, OFF);
    pinMode (pinRelayVitessesExterieur, OUTPUT);
    digitalWrite (pinRelayVitessesExterieur, OFF);
    pinMode(pinRelayVentilateurInterieur, OUTPUT);
    digitalWrite (pinRelayVentilateurInterieur, OFF);
    pinMode(pinRelayVitessesInterieur, OUTPUT);
    digitalWrite (pinRelayVitessesInterieur, OFF);
    pinMode(pinRelayEteHiver, OUTPUT);
    //digitalWrite (pinRelayEteHiver, OFF);
}

void Relay::highRelayEteHiver()
{
    TRACE("Relais ETE/HIVER = ON\n"); 
    digitalWrite (pinRelayEteHiver, ON);
}

void Relay::lowRelayEteHiver()
{
    TRACE("Relais ETE/HIVER = OFF\n");
    digitalWrite (pinRelayEteHiver, OFF);
}

void Relay::highRelayV4V()
{
    TRACE("Relais V4V = ON\n");
    digitalWrite (pinRelayV4V, ON);
}

void Relay::lowRelayV4V()
{
    TRACE("Relais V4V = OFF\n");
    digitalWrite (pinRelayV4V, OFF);
}

void Relay::highRelayCompresseur()
{
    digitalWrite (pinRelayCompresseur, ON);
}

void Relay::lowRelayCompresseur()
{
    digitalWrite (pinRelayCompresseur, OFF);
}

void Relay::highRelayVentilationInterieur()
{
    digitalWrite (pinRelayVentilateurInterieur, ON);
}

void Relay::lowRelayVentilationInterieur() 
{    
    digitalWrite (pinRelayVentilateurInterieur, OFF);
}

void Relay::highRelayGrandeVitesseInterieur() 
{
    digitalWrite (pinRelayVitessesInterieur, ON);
}

void Relay::lowRelayPetiteVitesseInterieur() 
{
    digitalWrite (pinRelayVitessesInterieur, OFF);
}

void Relay::highRelayVentilationExterieur() 
{    
    digitalWrite (pinRelayVentilateurExterieur, ON);
}

void Relay::lowRelayVentilationExterieur()
{
    digitalWrite (pinRelayVentilateurExterieur, OFF);
}

void Relay::highRelayGrandeVitesseExterieur() 
{
    digitalWrite (pinRelayVitessesExterieur, ON);
}

void Relay::lowRelayPetiteVitesseExterieur() 
{
    digitalWrite (pinRelayVitessesExterieur, OFF);
}

void Relay::lowAllRelay()
{
    digitalWrite (pinRelayEteHiver, OFF);
    digitalWrite (pinRelayV4V, OFF);
    digitalWrite (pinRelayCompresseur, OFF);
    digitalWrite (pinRelayVentilateurInterieur, OFF);
    digitalWrite (pinRelayVentilateurExterieur, OFF);
}

void Relay::testRelay()
{
    digitalWrite (pinRelayCompresseur, ON);
    digitalWrite (pinRelayV4V, ON);
    digitalWrite (pinRelayVentilateurExterieur, ON);
    digitalWrite (pinRelayVentilateurInterieur, ON);
    digitalWrite (pinRelayVitessesExterieur, ON);
    digitalWrite (pinRelayVitessesInterieur, ON);
    digitalWrite (pinRelayEteHiver, ON);
    
}
