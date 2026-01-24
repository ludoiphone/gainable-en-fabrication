#include "WeatherWidget.hpp"
#include <QDate>
#include <QLocale>

WeatherWidget::WeatherWidget(QWidget *parent, double lat, double lon, const QString &city)
    : QWidget(parent), m_lat(lat), m_lon(lon)
{
    // Taille du widget
    setFixedSize(500, 250);
    move(120, 40);

    // ==== Météo du jour ====
    m_cityLabel = new QLabel(city, this);
    m_cityLabel->setAlignment(Qt::AlignCenter);
    m_cityLabel->setStyleSheet("font-weight:bold; font-size:16px; color:#FFFFFF;");

    todayIcon = new QLabel(this);
    todayIcon->setAlignment(Qt::AlignCenter);
    todayIcon->setFixedSize(80, 80);
    todayIcon->setScaledContents(true);

    todayTemp = new QLabel("--°C", this);
    todayTemp->setAlignment(Qt::AlignCenter);
    todayTemp->setStyleSheet("font-size:18px; color:#FFFFFF;");

    // ==== Layout vertical pour la météo du jour ====
    QVBoxLayout *todayLayout = new QVBoxLayout;
    todayLayout->setContentsMargins(0, 0, 0, 0);
    todayLayout->setSpacing(5);
    todayLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    
    todayLayout->addWidget(m_cityLabel, 0, Qt::AlignHCenter);
    // Créer un layout intermédiaire pour l'icône + température
    QVBoxLayout *iconTempLayout = new QVBoxLayout;
    iconTempLayout->setContentsMargins(0,0,0,0);
    iconTempLayout->setSpacing(2);
    iconTempLayout->setAlignment(Qt::AlignHCenter);
    iconTempLayout->addWidget(todayIcon, 0, Qt::AlignHCenter);
    iconTempLayout->addWidget(todayTemp, 0, Qt::AlignHCenter);
    
    // Ajouter ce layout intermédiaire au layout principal
    todayLayout->addLayout(iconTempLayout);
    todayLayout->addStretch(1);

    // Widget pour englober météo du jour
    QWidget *todayWidget = new QWidget;
    todayWidget->setLayout(todayLayout);

    // ==== Prévisions 4 prochains jours ====
    forecastContainer = new QWidget;
    forecastLayout = new QHBoxLayout(forecastContainer);
    forecastLayout->setSpacing(10);
    forecastLayout->setContentsMargins(0,0,0,0);
    forecastLayout->setAlignment(Qt::AlignHCenter);

    // ==== Layout principal vertical ====
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(15);
    mainLayout->addWidget(todayWidget, 0, Qt::AlignHCenter);  // Météo du jour centrée
    mainLayout->addWidget(forecastContainer, 0, Qt::AlignHCenter); // Prévisions centrées

    // ==== WeatherManager ====
    m_weather = new WeatherManager(this);

    // Connexions
    connect(m_weather, &WeatherManager::currentWeatherReady, this, &WeatherWidget::onCurrent);
    connect(m_weather, &WeatherManager::forecastReady, this, &WeatherWidget::onForecast);
    connect(m_weather, &WeatherManager::iconReady, this, &WeatherWidget::onIcon);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &WeatherWidget::updateWeather);
    m_timer->start(10 * 60 * 1000);

    QTimer::singleShot(1000, this, &WeatherWidget::updateWeather);/////////
}

void WeatherWidget::updateWeather()
{
    m_weather->setApiKey("dbe2c1b47657bd094bc1ee26dae01154");
    
    auto tryFetch = [this]() {
        m_weather->fetchCurrent(m_lat, m_lon);
        m_weather->fetchForecast(m_lat, m_lon);
    };
    
    // Si erreur réseau, on retente après 2s
    connect(m_weather, &WeatherManager::errorOccurred, this, [this, tryFetch](QString msg){
        qDebug() << "[WeatherWidget] Erreur météo:" << msg << "- Retente dans 2s";
        QTimer::singleShot(2000, tryFetch);
    });

    tryFetch();
}

void WeatherWidget::onCurrent(QString city, double temp, QString desc, QString icon)
{
	Q_UNUSED(desc);
	Q_UNUSED(city);
	
    //m_cityLabel->setText(city);
    todayTemp->setText(QString::number(temp, 'f', 1) + "°C");
    m_todayIconCode = icon;
}

void WeatherWidget::onIcon(QString iconCode, QPixmap pixmap)
{
    // Icône du jour
    if (iconCode == m_todayIconCode)
        todayIcon->setPixmap(pixmap);

    // Icônes des 4 prochains jours
    for (auto &day : nextDays)
    {
        if (day.iconCode == iconCode)
            day.iconLabel->setPixmap(pixmap);
    }
}

void WeatherWidget::onForecast(QList<QVariantMap> list)
{
    // Supprimer anciens widgets
    QLayoutItem *child;
    while ((child = forecastLayout->takeAt(0)) != nullptr)
    {
        if (child->widget()) delete child->widget();
        delete child;
    }
    nextDays.clear();

    QLocale french(QLocale::French);
    int count = 0;
    
    for (const auto &m : list)
    {
        QDate dt = QDate::fromString(m["datetime"].toString(), "yyyy-MM-dd");

        // Layout vertical pour chaque jour
        QVBoxLayout *vbox = new QVBoxLayout;
        vbox->setSpacing(2);
        vbox->setContentsMargins(0,0,0,0);
        vbox->setAlignment(Qt::AlignHCenter);

        QLabel *dateLabel = new QLabel(french.dayName(dt.dayOfWeek(), QLocale::ShortFormat) + " " + QString::number(dt.day()));
        //QLabel *dateLabel = new QLabel(dt.toString("ddd dd MMM"));
        dateLabel->setAlignment(Qt::AlignCenter);
        dateLabel->setStyleSheet("font-size:12px;");
        
        QLabel *iconLabel = new QLabel;
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setFixedSize(60,60);
        iconLabel->setScaledContents(true);
        
        QLabel *tempLabel = new QLabel(QString::number(m["temp"].toDouble(), 'f', 1) + "°C");
        tempLabel->setAlignment(Qt::AlignCenter);

        vbox->addWidget(dateLabel);
        vbox->addWidget(iconLabel);
        vbox->addWidget(tempLabel);

        QWidget *dayWidget = new QWidget;
        dayWidget->setLayout(vbox);
        dayWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        forecastLayout->addWidget(dayWidget);

        DayWidget day;
        day.date = dt;
        day.iconLabel = iconLabel;
        day.tempLabel = tempLabel;
        day.iconCode = m["icon"].toString();
        nextDays.append(day);

        if (++count >= 4) break;
    }

    forecastLayout->addStretch();
}

