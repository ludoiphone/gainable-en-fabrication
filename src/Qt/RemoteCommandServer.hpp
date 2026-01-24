#pragma once

#include <QObject>
#include <QCoreApplication> 
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>
#include <QString>
#include "Qt/AppWindow.hpp"
#include "core/PacController.hpp"
#include "utils/Log.hpp"

class RemoteCommandServer : public QObject
{
    Q_OBJECT

public:
    RemoteCommandServer(AppWindow* window, PacController* pac, QObject* parent = nullptr);
    ~RemoteCommandServer();

private:
    AppWindow* m_window;
    PacController* m_pac;
    QLocalServer* m_server;

    void processCommand(const QString& cmd);

private slots:
    void handleNewConnection();
    void handleReadyRead();
};

