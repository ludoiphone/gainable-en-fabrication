#include "VentilateurExterieur.hpp"

VentilateurExterieur::VentilateurExterieur(int pinOn, int pinV2)
    : m_on(pinOn),
      m_v2(pinV2),
      m_speed(VentExtSpeed::OFF)
{
}

void VentilateurExterieur::set(VentExtSpeed speed)
{
    if (speed == m_speed)
        return;

    switch (speed) {
    case VentExtSpeed::OFF:
        m_v2.off();
        m_on.off();
        break;

    case VentExtSpeed::V1:
        m_on.on();
        m_v2.off();
        break;

    case VentExtSpeed::V2:
        m_on.on();
        m_v2.on();
        break;
    }

    m_speed = speed;
}

VentExtSpeed VentilateurExterieur::speed() const
{
    return m_speed;
}

bool VentilateurExterieur::isOn() const
{
    return m_speed != VentExtSpeed::OFF;
}

