#include "TemperatureWidget.hpp"
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>

TemperatureWidget::TemperatureWidget(PacController *pac,  QWidget *parent)
    : QWidget(parent), m_pac(pac)
{
    setGeometry(0, 0, 1024, 600); // zone totale identique à MainWindow

    QStringList sensorNames = {
        "Extérieur :", "Unité Extérieure :", "Échangeur Extérieur :",
        "Unité Intérieure :", "Échangeur Intérieur :"
    };

    int sensorCount = sensorNames.size();
    m_displays.resize(sensorCount);

    // ----- CHARGEMENT FONTS -----
    int fontId = QFontDatabase::addApplicationFont("/home/ludo/Desktop/PAC2/fonts/digital-7.ttf");
    QFont digitalFont;
    if (fontId != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty())
            digitalFont = QFont(families.at(0), 40);
    }

    // ----- REPRISE EXACTE DES POSITIONS ORIGINALES -----
    int yStart = 83;
    int yEnd   = 478;
    double step = (yEnd - yStart) / double(sensorCount - 1);

    for (int i = 0; i < sensorCount; i++) {

        int yTitre = yStart + int(i * step);
        int yTemp  = yTitre + 22;

        // ------- TITRE -------
        QLabel *title = new QLabel(sensorNames[i], this);
        title->setGeometry(26, yTitre, 210, 22);
        title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        title->setStyleSheet(
            "font-family:'Coolvetica'; font-size:10pt; color:white; background:transparent;"
        );

        // ------- VALEUR -------
        QLabel *value = new QLabel("--°C", this);
        value->setGeometry(26, yTemp, 200, 44);
        value->setFont(digitalFont);
        value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        value->setStyleSheet("color:green; background:transparent;");

        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(0);
        shadow->setOffset(1, 1);
        shadow->setColor(Qt::black);
        value->setGraphicsEffect(shadow);

        m_displays[i] = { title, value };
    }

    // Timer MAJ
    m_updateTemps = new QTimer(this);
    connect(m_updateTemps, &QTimer::timeout, this, &TemperatureWidget::refreshTemperatures);
    m_updateTemps->start(2000);
}

void TemperatureWidget::refreshTemperatures()
{
    if (!m_pac) return;

    QStringList keys = {"Ext","UExt","EExt","UInt","EInt"};

    for (int i = 0; i < m_displays.size(); ++i) {
        double val = m_pac->getTemperature(keys[i].toStdString());
        m_displays[i].value->setText(QString("%1°C").arg(val,0,'f',1));

        QString color = val < 10 ? "blue" : val < 25 ? "green" :
                        val < 35 ? "orange" : "red";

        m_displays[i].value->setStyleSheet(QString("color:%1; background:transparent;").arg(color));
    }
}

