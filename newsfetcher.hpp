#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QStringList>

class NewsFetcher : public QObject
{
    Q_OBJECT
public:
    explicit NewsFetcher(QObject *parent = nullptr);

public slots:
    void start();
    void fetchNews();

signals:
    void newsReady(QStringList titles);

private:
    QNetworkAccessManager *m_manager;
    QTimer *m_timer;
};
