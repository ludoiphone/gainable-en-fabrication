#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "hardware/NTCReader.hpp"
#include "Qt/AppWindow.hpp"
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

    if (wiringPiSetupGpio() == -1) {
        return 1;
    }

    std::signal(SIGINT, signalHandler);

    Consignes consignes;
    Temporisations temporisations;

    NTCReader sondes("/home/ludo/Desktop/Gainable/config/configNTC.ini");
    sondes.start();

    PacController pac(sondes, consignes, temporisations);

    AppWindow w(&pac);

    QApplication::setOverrideCursor(Qt::BlankCursor);
    w.setWindowTitle("Supervision PAC");
    w.setFixedSize(1024, 600);
    w.showFullScreen();

    int ret = app.exec();

    pac.stopPac();
    sondes.stop();

    return ret;
}


