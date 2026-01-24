#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QLocalServer>
#include <QLocalSocket>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include "utils/Log.hpp"
#include "hardware/NTCReader.hpp"
#include "Qt/AppWindow.hpp"
#include "Qt/RemoteCommandServer.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include "core/PacController.hpp"
#include "hardware/RelayManager.hpp"
#include "hardware/SwitchManager.hpp"

void signalHandler(int) {
    LOG_WARN("[SYSTEM] arrêt sécurisé par l'administrateur");
    QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
    Log::init("/home/ludo/Desktop/PAC2/config/log4cxx.properties");
    
    QApplication app(argc, argv);
    LOG_INFO("[SYSTEN] Application PAC démarrée");

    if (wiringPiSetupGpio() == -1) {
        LOG_ERROR("[SYSTEM] Échec de l'initialisation des GPIO avec wiringPi !");
        return 1;
    }

    std::signal(SIGINT, signalHandler);
    
    // ================ Initialisation des modules ==============
    
    Consignes consignes;
    //LOG_INFO("[PACCONTOLLER] Initialisation des Temporisations");
    Temporisations temporisations;
    //LOG_INFO("[PACCONTROLLER] Initialisatio des Consignes");
    
    LOG_INFO("[SYSTEM] Initialisation des sondes NTC");
    NTCReader sondes("/home/ludo/Desktop/PAC2/config/configNTC.ini");
    sondes.start();
    
    LOG_INFO("[SYSTEM] Initialisation des relais");
    RelayManager relays;
    
    LOG_INFO("[SYSTEM] Initialisation des swtichs");
    SwitchManager switchs;
    
    // =============== Initialisation PAC ================
    LOG_INFO("[SYSTEM] Initialisation du PacController");
    PacController pac(sondes, relays, switchs, consignes, temporisations);

    // ================ Fenêtre principale ================
    LOG_INFO("[SYSTEM] Création de la fenêtre principale");
    AppWindow w(&pac);

    QApplication::setOverrideCursor(Qt::BlankCursor);
    w.setWindowTitle("Supervision PAC");
    w.setFixedSize(1024, 600);
    w.showFullScreen();
    
    // ================= Commande terminale ServiceTest =================
    RemoteCommandServer remote(&w, &pac);

    int ret = app.exec();

    // ================ Nettoyage ==============
    LOG_INFO("[SYSTEM] Arrêt des modules PAC et sondes");
    pac.stopPac();
    sondes.stop();

    LOG_INFO("[SYSTEM] Application PAC terminée");
    return ret;
}


