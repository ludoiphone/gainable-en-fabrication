#include "SwitchManager.hpp"

// ====================
// Constructeur
// ====================
SwitchManager::SwitchManager()
    : thermostat(22),
      filtre(27)
{
}

// ====================
// Accès directs
// ====================
bool SwitchManager::thermostatClosed() const
{
    return thermostat.isActive();
}

bool SwitchManager::filtreOpen() const
{
    return filtre.isActive();
}
