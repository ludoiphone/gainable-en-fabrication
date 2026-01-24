#pragma once
#include "hardware/VentilateurExterieur.hpp"
#include "hardware/VentilateurInterieur.hpp"

enum ModePAC {
    MODE_ARRET,
    MODE_CHAUFFAGE,
    MODE_FROID,
    MODE_CANICULE
};

class PacStatusUI {
    
public:
    // ================= ÉTATS =================
    ModePAC mode = MODE_ARRET;
    
    bool stabilisationActif = false;

    bool compresseurActif = false;
    bool ventExtActif = false;
    VentExtSpeed ventExtVitesse = VentExtSpeed::OFF;
    bool ventIntActif = false;
    VentIntSpeed ventIntVitesse = VentIntSpeed::OFF;

    bool degivrageActif = false;
    bool egouttageActif = false;

    bool filtreANettoyer = false;
    bool filtreNettoyageEnCours = false;
    bool filtreArretAbsolu = false;

    bool transitionSaisons = false;
    bool canicule = false;
    bool ete = false;
    bool hiver = false;

    // ================= UTILS =================
    void reset();
};

