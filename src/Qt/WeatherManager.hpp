#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>

class WeatherManager : public QObject
{
    Q_OBJECT
public:
    explicit WeatherManager(QObject *parent = nullptr);

    void setApiKey(const QString &key);
    void fetchCurrent(double lat, double lon);
    void fetchForecast(double lat, double lon);

signals:
    void currentWeatherReady(QString city, double temp, QString description, QString iconCode);
    void forecastReady(QList<QVariantMap> list);
    void iconReady(QString iconCode, QPixmap pixmap);
    void errorOccurred(QString message);

private slots:
    void onCurrentReply(QNetworkReply *reply);
    void onIconReply(QNetworkReply *reply);

private:
    void downloadIcon(const QString &iconCode);

    QString m_apiKey;
    QNetworkAccessManager m_net;
};

