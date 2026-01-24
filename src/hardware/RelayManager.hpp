#pragma once

#include "Output.hpp"
#include "VentilateurExterieur.hpp"
#include "VentilateurInterieur.hpp"
#include "utils/Log.hpp"

class RelayManager {
public:
    RelayManager();

    // Actionneurs
    VentilateurExterieur ventExt;
    VentilateurInterieur ventInt;

    Output compresseur;
    Output vanne4V;
    Output eteHiver;

    // Sécurité : tout couper
    void setCompresseur(bool on);
    void setVanne4V(bool on);
    void setEteHiver(bool saisons, bool serviceTest);
    void setVentExt(VentExtSpeed speed);
    void setVentInt(VentIntSpeed speed);
    
    void allOff();
    
};

