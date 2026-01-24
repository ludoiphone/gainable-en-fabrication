#include "WeatherManager.hpp"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

WeatherManager::WeatherManager(QObject *parent)
    : QObject(parent)
{
}

void WeatherManager::setApiKey(const QString &key)
{
    m_apiKey = key;
}

void WeatherManager::fetchCurrent(double lat, double lon)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key manquante");
        return;
    }

    QUrl url("https://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;
    query.addQueryItem("lat", QString::number(lat));
    query.addQueryItem("lon", QString::number(lon));
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");
    query.addQueryItem("lang", "fr");
    url.setQuery(query);

    QNetworkReply *reply = m_net.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onCurrentReply(reply);
    });
}

void WeatherManager::fetchForecast(double lat, double lon)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key manquante");
        return;
    }

    QUrl url("https://api.openweathermap.org/data/2.5/forecast");
    QUrlQuery query;
    query.addQueryItem("lat", QString::number(lat));
    query.addQueryItem("lon", QString::number(lon));
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");
    query.addQueryItem("lang", "fr");
    url.setQuery(query);

    QNetworkReply *reply = m_net.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) return;

        QJsonArray list = doc.object()["list"].toArray();
        QMap<QDate, QVariantMap> dailyMap;
        QDate today = QDate::currentDate();

        for (auto v : list) {
            QJsonObject obj = v.toObject();
            QDateTime dt = QDateTime::fromSecsSinceEpoch(obj["dt"].toInt());
            QDate date = dt.date();

            if (date <= today) continue; // ignorer aujourd'hui

            if (!dailyMap.contains(date)) {
                QVariantMap map;
                map["datetime"] = date.toString("yyyy-MM-dd");
                map["temp"] = obj["main"].toObject()["temp"].toDouble();
                map["icon"] = obj["weather"].toArray()[0].toObject()["icon"].toString();
                dailyMap[date] = map;

                downloadIcon(map["icon"].toString());
            }
        }

        QList<QVariantMap> result = dailyMap.values();
        emit forecastReady(result);
    });
}

void WeatherManager::onCurrentReply(QNetworkReply *reply)
{
    if (reply->error()) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject o = doc.object();

    QString city = o["name"].toString();
    double temp = o["main"].toObject()["temp"].toDouble();
    QString desc = o["weather"].toArray()[0].toObject()["description"].toString();
    QString icon = o["weather"].toArray()[0].toObject()["icon"].toString();

    emit currentWeatherReady(city, temp, desc, icon);

    downloadIcon(icon);
}

void WeatherManager::downloadIcon(const QString &iconCode)
{
    QString url = QString("https://openweathermap.org/img/wn/%1@2x.png").arg(iconCode);
    QNetworkReply *reply = m_net.get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onIconReply(reply);
    });
}

void WeatherManager::onIconReply(QNetworkReply *reply)
{
    if (reply->error()) {
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QPixmap pix;
    pix.loadFromData(data);

    QString iconCode = reply->url().path().section('/', -1).section('@', 0, 0);
    emit iconReady(iconCode, pix);

    reply->deleteLater();
}

