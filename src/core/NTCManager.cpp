#include "NTCManager.hpp"
#include "utils/Log.hpp"
#include <cmath>

NTCManager::NTCManager(NTCReader& sondes)
    : m_sondes(sondes), m_fault(false)
{
}

void NTCManager::update()
{
    m_fault = false;

    for (const auto& name : {"Ext", "UExt", "EExt", "UInt", "EInt"}) {
        bool oldValid = isValid(name);
        
        int idx = m_sondes.getChannelIndex(name);
        if (idx < 0) {
            if (oldValid) {
                LOG_ERROR("[NTCMANAGER] Sonde inconnue : " + std::string(name));
            }
            m_validSondes[name] = false;
            m_fault = true;
            continue;
        }

        int adc = m_sondes.getADC(idx);
        double volt = m_sondes.getVoltage(idx);
        double r = m_sondes.getResistance(idx);
        double t = m_sondes.getTemperature(name);

        bool valid = true;

        if (std::isnan(t)) valid = false;
        if (t < -40 || t > 120) valid = false;
        if (adc <= 50 || adc >= 4000) valid = false;
        if (volt < 0.05 || volt > 3.2) valid = false;
        if (r < 2000 || r > 60000) valid = false;

        if (valid != oldValid) {
            if (!valid)
                LOG_WARN("[NTCMANAGER] Sonde " + std::string(name) + " invalide t=" + std::to_string(t));
            else 
                LOG_INFO("[NTCMANAGER] Sonde " + std::string(name) + " redevenue valide");
        }
        
        if (!valid) m_fault = true;
        m_validSondes[name] = valid;
    }
}

bool NTCManager::isValid(const std::string& name) const
{
    auto it = m_validSondes.find(name);
    return it != m_validSondes.end() && it->second;
}

double NTCManager::getTemperature(const std::string& name) const
{
    if (!isValid(name)) return NAN;
    return m_sondes.getTemperature(name);
}

bool NTCManager::hasFault() const
{
    return m_fault;
}
