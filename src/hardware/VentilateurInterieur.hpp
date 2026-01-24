#pragma once

#include "Output.hpp"

enum class VentIntSpeed {
    OFF,
    V1,
    V2,
    V3,
    V4   // grande vitesse intérieur
};

class VentilateurInterieur {
public:
    VentilateurInterieur(int pinOn, int pinV4);

    void set(VentIntSpeed speed);
    VentIntSpeed speed() const;

    bool isOn() const;

private:
    Output m_on;
    Output m_v4;

    VentIntSpeed m_speed;
};
