#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include "WeatherManager.hpp"

class WeatherWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WeatherWidget(QWidget *parent = nullptr, 
                           double lat = 46.8217,
                           double lon = -1.6489,
                           const QString &city = "Grand'Landes");

private:
    WeatherManager *m_weather;
    QTimer *m_timer;
    double m_lat, m_lon;

    QLabel *m_cityLabel;
    QLabel *todayTemp;
    QLabel *todayIcon;
    QString m_todayIconCode;

    QWidget *forecastContainer;
    QHBoxLayout *forecastLayout;

    struct DayWidget {
        QDate date;
        QLabel *iconLabel;
        QLabel *tempLabel;
        QString iconCode;
    };
    QList<DayWidget> nextDays;

private slots:
    void updateWeather();
    void onCurrent(QString city, double temp, QString desc, QString icon);
    void onIcon(QString iconCode, QPixmap pixmap);
    void onForecast(QList<QVariantMap> list);
};

