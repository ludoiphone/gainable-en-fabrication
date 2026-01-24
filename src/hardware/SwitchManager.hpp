#pragma once

#include "Input.hpp"

class SwitchManager {
public:
    SwitchManager();

    // Accès simples
    bool thermostatClosed() const;
    bool filtreOpen() const;

private:
    Input thermostat;
    Input filtre;
};
