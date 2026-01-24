#pragma once
#include "hardware/VentilateurExterieur.hpp"
#include "hardware/VentilateurInterieur.hpp"

class PacStatusService {
	
public:
    bool serviceActif = false;

    // Relais
    bool eteHiver = false;
    bool compresseur = false;
    bool vanne4V = false;

    // Ventilateurs
    bool ventExtActif = false;
    VentExtSpeed ventExtVitesse = VentExtSpeed::OFF;

    bool ventIntActif = false;
    VentIntSpeed ventIntVitesse = VentIntSpeed::OFF;

    // Entrées
    bool thermostat = false;
    bool filtreOpen = false;
    
    void reset();
};
