#include "VentilateurInterieur.hpp"

VentilateurInterieur::VentilateurInterieur(int pinOn, int pinV4)
    : m_on(pinOn),
      m_v4(pinV4),
      m_speed(VentIntSpeed::OFF)
{
}

void VentilateurInterieur::set(VentIntSpeed speed)
{
    if (speed == m_speed)
        return;

    switch (speed) {
    case VentIntSpeed::OFF:
        m_v4.off();
        m_on.off();
        break;

    case VentIntSpeed::V1:
        m_on.on();
        m_v4.off();
        break;
    case VentIntSpeed::V2:
    case VentIntSpeed::V3:
    case VentIntSpeed::V4:
        m_on.on();
        m_v4.on();
        break;
    }

    m_speed = speed;
}

VentIntSpeed VentilateurInterieur::speed() const
{
    return m_speed;
}

bool VentilateurInterieur::isOn() const
{
    return m_speed != VentIntSpeed::OFF;
}
