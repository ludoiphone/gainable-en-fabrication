#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <string>

#include "hardware/NTCReader.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include "hardware/Chauffage.hpp"
#include "hardware/Froid.hpp"
#include "hardware/RelayManager.hpp"
#include "hardware/SwitchManager.hpp"
#include "FiltreManager.hpp"
#include "NTCManager.hpp"
#include "PacStatusUI.hpp"
//#include "PacStatusService.hpp"

// ================= FSM PAC =================
enum class PacState {
    INIT_SONDES,
    ATTENTE_VOLET,
    MARCHE,
    PAUSE,
    SECURITE,
    SERVICE
};

class PacController {

public:
    PacController(NTCReader& sondes, RelayManager& relays, SwitchManager& switchs, Consignes& consignes, Temporisations& temporisations);
    ~PacController();
    
    double getTemperature(const std::string& name) const;
    
    void pausePac();
    void pauseUserPac();
    void restartUserPac();
    void stopPac();
    
    PacStatusUI getStatusUI() const;
    //PacStatusService getStatusService() const;
    
    // Activer / désactiver mode service
    void serviceEnable(bool enable);
    
    // Forcer les relais
    void serviceSetRelayEteHiver(bool on);
    void serviceSetRelayCompresseur(bool on);
    void serviceSetV4V(bool on);
    void serviceSetVentilateurExt(VentExtSpeed speed);
    void serviceSetVentilateurInt(VentIntSpeed speed);
    
    // Arrêt sécurité
    void serviceAllOff();
    
    bool thermostatOn() const;
    bool isFiltreOpen() const;

private:
    // ================= DEPENDANCES =================
    NTCReader&       m_sondes;
    NTCManager       m_ntcManager;
    SwitchManager&   m_switch;
    RelayManager&    m_relay;
    Consignes&       m_consignes;
    Temporisations&  m_tempos;

    Chauffage        m_chauffage;
    Froid            m_froid;
    
    FiltreManager    m_filtreManager;
    PacStatusUI      m_pacStatusUI;
    //PacStatusService m_pacStatusService;
    
    // ================= FSM =================
    PacState m_state;
    PacState m_statePrev;
    ModePAC  m_modeActuel;

    std::chrono::steady_clock::time_point m_stateStart;
    std::chrono::steady_clock::time_point m_lastTime;

    std::thread m_thread;
    std::atomic<bool> m_stopAdmin;
    std::atomic<bool> m_userPause;
    std::atomic<bool> m_serviceTest;
    
    bool m_ntcInitDone;
    bool m_stabilisationNtc;
    bool m_lastNtcFault;
    bool m_lastUserRun;
    bool m_lastThermostatRequest;

    // ================= FSM CORE =================
    void loop();
    void update();
    void handleState();
    void enterState(PacState newState);

    // ================= ACTIONS =================
    void actionInitSondes();
    void actionAttentVolet();
    void actionMarche();
    void actionPause();
    void actionSecurite();
    void actionService();

    // ================= HELPERS =================
    ModePAC determineMode() const;
    void applySeasonRelay(ModePAC mode);
    bool needVoletTransition(ModePAC actuel, ModePAC voulu) const;
};

