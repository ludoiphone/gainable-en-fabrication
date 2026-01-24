#include "PacStatusUI.hpp"

void PacStatusUI::reset()
{
    mode = MODE_ARRET;
    stabilisationActif = false;
    compresseurActif = false;
    ventExtActif = false;
    ventExtVitesse = VentExtSpeed::OFF;
    ventIntActif = false;
    ventIntVitesse = VentIntSpeed::OFF;
    degivrageActif = false;
    egouttageActif = false;
    filtreANettoyer = false;
    filtreNettoyageEnCours = false;
    filtreArretAbsolu = false;
    transitionSaisons = false;
    canicule = false;
    ete = false;
    hiver = false;
}

