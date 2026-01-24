#include "RemoteCommandServer.hpp"

RemoteCommandServer::RemoteCommandServer(AppWindow* window, PacController* pac, QObject* parent)
    : QObject(parent), m_window(window), m_pac(pac)
{
    m_server = new QLocalServer(this);

    // Nettoyage si crash précédent
    QLocalServer::removeServer("/tmp/pac_cmd");

    if (!m_server->listen("/tmp/pac_cmd")) {
        LOG_ERROR("[REMOTECOMMANDESERVER] Impossible de créer le serveur de commandes à distance");
        return;
    }

    connect(m_server, &QLocalServer::newConnection, this, &RemoteCommandServer::handleNewConnection);
    LOG_INFO("[REMOTECOMMANDESERVER] Serveur de commandes à distance démarré");
}

RemoteCommandServer::~RemoteCommandServer()
{
    m_server->close();
    QLocalServer::removeServer("/tmp/pac_cmd");
}

void RemoteCommandServer::handleNewConnection()
{
    QLocalSocket* client = m_server->nextPendingConnection();
    if (!client) return;

    connect(client, &QLocalSocket::readyRead, this, &RemoteCommandServer::handleReadyRead);
}

void RemoteCommandServer::handleReadyRead()
{
    QLocalSocket* client = qobject_cast<QLocalSocket*>(sender());
    if (!client || !client->isOpen()) return;

    QString cmd = QString::fromUtf8(client->readAll()).trimmed();
    processCommand(cmd);

    client->disconnectFromServer();
}

void RemoteCommandServer::processCommand(const QString& cmd)
{
    if (cmd == "serviceTest") {
        //LOG_INFO("[MAIN] " + cmd.toStdString() + " par administrateur");
        if (m_window && m_pac) {
            m_pac->serviceEnable(true);//pausePac();
            QMetaObject::invokeMethod(m_window, [&]() {
                m_window->showServiceTest();
            }, Qt::QueuedConnection);
        }
    } else if (cmd == "modifConsignes") {
        LOG_INFO("[MAIN] Modification consignes par administrateur");
        if (m_window) {
            QMetaObject::invokeMethod(m_window, [&]() {
                m_window->showModifCons();
            }, Qt::QueuedConnection);
        }
    } else if (cmd == "quitPAC") {
        LOG_INFO("[MAIN] arrêt de l'application par administrateur");
        QCoreApplication::exit(0);
    } else if (cmd == "rebootPAC") {
        LOG_INFO("[MAIN] redémarrage de l'application par administrateur");
        QCoreApplication::exit(10);
    } else {
        LOG_WARN("[MAIN] commande inconnue : " + cmd.toStdString());
    }
}

