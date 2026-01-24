#pragma once
#include "Output.hpp"

enum class VentExtSpeed {
    OFF,
    V1,
    V2   // grande vitesse extérieur
};

class VentilateurExterieur {
	
public:
    VentilateurExterieur(int pinOn, int pinV2);

    void set(VentExtSpeed speed);
    VentExtSpeed speed() const;

    bool isOn() const;
    
private:
    Output m_on;
    Output m_v2;

    VentExtSpeed m_speed;
};
