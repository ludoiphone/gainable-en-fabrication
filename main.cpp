#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

#include "hardware/DS18B20.hpp"
#include "ui/appwindow.hpp"
#include "utils/Logger.hpp"
#include "logic/Consignes.hpp"
#include "logic/Temporisations.hpp"
#include "core/PacController.hpp"

void signalHandler(int) {
    std::cout << "[SYSTEM] Ctrl+C détecté — arrêt sécurisé\n";
    QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Logger logger("pac.log");
    logger.info("Démarrage du monitoring DS18B20...");

    if (wiringPiSetupGpio() == -1) {
        logger.error("[SYSTEM] Erreur init GPIO !");
        return 1;
    }

    std::signal(SIGINT, signalHandler);
    
    Consignes consignes;
    Temporisations temporisations;
    
    DS18B20 sondes(logger, 5);
    sondes.start();

    PacController pac(logger, sondes, consignes, temporisations);

    AppWindow w(&pac);

    QApplication::setOverrideCursor(Qt::BlankCursor);
    w.setWindowTitle("Supervision PAC");
    w.setFixedSize(1024, 600);
    w.showFullScreen();

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    w.move((screenGeometry.width() - w.width()) / 2,
           (screenGeometry.height() - w.height()) / 2);


    int ret = app.exec();

    pac.stopPac();
    sondes.stop();
    logger.info("[SYSTEM] Arrêt terminé, sortie du programme");

    return ret;
}


