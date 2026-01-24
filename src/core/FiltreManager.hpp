#pragma once
#include "logic/Temporisations.hpp"

class FiltreManager {
public:
    FiltreManager(Temporisations& tempos);

    void update(double dt, bool ventiloIntActif, bool pauseGlobale, bool filtreOpen);

    bool nettoyageEnCours() const;
    bool alerteNettoyage() const;
    bool arretAbsolu() const;

    void reset();

private:
    Temporisations& m_tempos;

    bool m_filtreInfoLogged = false;
    bool m_filtreStopLogged = false;
    bool m_stopFiltre = false;
    bool m_prevFilterOpen = false;
};
