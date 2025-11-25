#include "MainWindow.hpp"
#include <QLabel>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

// ===================== Interface des températures =====================
void MainWindow::setupTemperatureInterface()
{
    QStringList sensorNames = {
        "Extérieur :", "Unité Extérieure :", "Échangeur Extérieur :",
        "Unité Intérieure :", "Échangeur Intérieur :"
    };

    const int sensorCount = sensorNames.size();
    if (sensorCount != 5) {
        qDebug() << "Erreur: nombre de capteurs inattendu";
        return;
    }

    // --- Chargement police numérique ---
    int fontId = QFontDatabase::addApplicationFont("/home/ludo/Qt/test4/fonts/digital-7.ttf");
    QFont digitalFont;
    if (fontId != -1) {
        QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        if (!families.isEmpty()) {
            digitalFont = QFont(families.at(0), 40);
        }
    }

    int yStart = 83;
    int yEnd   = 478;
    double step = (yEnd - yStart) / double(sensorCount - 1);

    for (int i = 0; i < sensorCount; i++) {
        int yTitre = yStart + int(i * step);
        int yTemp  = yTitre + 22;

        // ===== Label titre =====
        m_titreTemps[i] = new QLabel(sensorNames[i], this);
        m_titreTemps[i]->setGeometry(26, yTitre, 200, 22);
        m_titreTemps[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_titreTemps[i]->setStyleSheet(
            "font-family:'Coolvetica'; font-size:10pt; color:white; background:transparent;"
        );

        // ===== Texte température =====
        m_tempDisplays[i].text = new QLabel(this);
        m_tempDisplays[i].text->setGeometry(26, yTemp, 200, 44);
        m_tempDisplays[i].text->setFont(digitalFont);
        m_tempDisplays[i].text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_tempDisplays[i].text->setStyleSheet("color:green; background:transparent;");
        
        // ===== Contour noir (léger) =====
        QGraphicsDropShadowEffect *outline = new QGraphicsDropShadowEffect(this);
        outline->setBlurRadius(0);            // pas de flou
        outline->setOffset(1, 1);             // petit décalage pour effet de contour
        outline->setColor(Qt::black);         // contour noir
        m_tempDisplays[i].text->setGraphicsEffect(outline);
    }

    qDebug() << "[UI] Interface températures initialisée correctement";
}

// ===================== Mise à jour des températures =====================
void MainWindow::updateTemperature(int index, double value)
{
    if (index < 0 || index >= 5) return;
    if (!m_tempDisplays[index].text) return;

    // Texte numérique
    m_tempDisplays[index].text->setText(QString("%1°C").arg(value, 0, 'f', 1));

    // Couleur selon la valeur
    QString color;
    if (value < 10)
        color = "blue";
    else if (value < 25)
        color = "green";
    else if (value < 35)
        color = "orange";
    else
        color = "red";

    m_tempDisplays[index].text->setStyleSheet(QString("color:%1; background:transparent;").arg(color));
}
