#include "PacStatusService.hpp"

void PacStatusService::reset() 
{
	
    serviceActif = false;
    // Relais
    eteHiver = false;
    compresseur = false;
    vanne4V = false;
    // Ventilateurs
    ventExtActif = false;
    ventExtVitesse = VentExtSpeed::OFF;
    ventIntActif = false;
    ventIntVitesse = VentIntSpeed::OFF;
    // Entrées
    thermostat = false;
    filtreOpen = false;
}
