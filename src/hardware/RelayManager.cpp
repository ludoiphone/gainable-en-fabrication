#include "RelayManager.hpp"

// ====================
// Constructeur
// ====================
RelayManager::RelayManager()
    : ventExt(13, 16), ventInt(19, 20), compresseur(5),
      vanne4V(6), eteHiver(26)
{
}

void RelayManager::setCompresseur(bool on)
{
    if (compresseur.isOn() == on)
        return;

    on ? compresseur.on() : compresseur.off();

    LOG_WARN(std::string("[RELAYMANAGER] Compresseur -> ") + (on ? "ON" : "OFF"));
}

void RelayManager::setVanne4V(bool on)
{
    if (vanne4V.isOn() == on)
        return;

    on ? vanne4V.on() : vanne4V.off();

    LOG_WARN(std::string("[RELAYMANAGER] Vanne 4 voies -> ") + (on ? "ON" : "OFF"));
}

void RelayManager::setEteHiver(bool saisons, bool serviceTest)
{
    if (eteHiver.isOn() == saisons)
        return;

    saisons ? eteHiver.on() : eteHiver.off();
    
    if (serviceTest) {
        LOG_WARN(std::string("[RELAYMANAGER] Relais Été/Hiver -> ") + (saisons ? "ON" : "OFF"));
    } else {
        LOG_WARN(std::string("[RELAYMANAGER] Relais Été/Hiver -> ") + (saisons ? "HIVER" : "ÉTÉ"));
    }
}

void RelayManager::setVentExt(VentExtSpeed speed)
{
    if (ventExt.speed() == speed)
        return;

    ventExt.set(speed);

    const char* txt =
        (speed == VentExtSpeed::OFF) ? "OFF" :
        (speed == VentExtSpeed::V1)  ? "V1"  :
        (speed == VentExtSpeed::V2)  ? "V2"  : "?";

    LOG_WARN(std::string("[RELAYMANAGER] Ventilateur Ext -> ") + txt);
}

void RelayManager::setVentInt(VentIntSpeed speed)
{
    if (ventInt.speed() == speed)
        return;

    ventInt.set(speed);

    const char* txt =
        (speed == VentIntSpeed::OFF) ? "OFF" :
        (speed == VentIntSpeed::V1)  ? "V1"  :
        (speed == VentIntSpeed::V4)  ? "V4"  : "?";

    LOG_WARN(std::string("[RELAYMANAGER] Ventilateur Int -> ") + txt);
}

// ====================
// Sécurité globale
// ====================
void RelayManager::allOff()
{
    bool any =
        ventExt.isOn() ||
        ventInt.isOn() ||
        compresseur.isOn() ||
        vanne4V.isOn() ||
        eteHiver.isOn();

    setVentExt(VentExtSpeed::OFF);
    setVentInt(VentIntSpeed::OFF);

    setCompresseur(false);
    setVanne4V(false);
    setEteHiver(false, false);
    
    if (any)
        LOG_WARN("[RELAYMANAGER] Arrêt GLOBAL des relais");
}
