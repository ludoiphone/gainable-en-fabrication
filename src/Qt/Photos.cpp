#include "Photos.hpp"
#include <QDir>
#include <QImageReader>
#include <QPainter>
#include <QPainterPath>

Photos::Photos(const QString &folderPath, QWidget *parent)
    : QWidget(parent), m_folderPath(folderPath)
{
    m_photoLabel = new QLabel(this);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setScaledContents(false);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Photos::showNextPhoto);

    loadPhotos();

    if (!m_pixmaps.isEmpty()) {
        m_photoLabel->setPixmap(m_pixmaps[0]);
        m_photoLabel->show();
        m_timer->start(10000); // intervalle par défaut 10s
    }
}

void Photos::setInterval(int ms)
{
    if (m_timer)
        m_timer->start(ms);
}

void Photos::loadPhotos()
{
    const int labelW = 400;
    const int labelH = 400;

    QDir dir(m_folderPath);
    QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
    m_photoFiles = dir.entryList(filters, QDir::Files);

    for (const QString &file : m_photoFiles) {
        QString path = m_folderPath + "/" + file;

        QImageReader reader(path);
        reader.setAutoTransform(true);
        reader.setScaledSize(QSize(labelW, labelH));
        QImage img = reader.read();
        if (!img.isNull()) {
            // coins arrondis
            QPixmap rounded(labelW, labelH);
            rounded.fill(Qt::transparent);
            QPainter painter(&rounded);
            painter.setRenderHint(QPainter::Antialiasing);
            QPainterPath path;
            path.addRoundedRect(0, 0, labelW, labelH, 30, 30);
            painter.setClipPath(path);
            painter.drawImage(0, 0, img);
            m_pixmaps.append(rounded);
        }
    }
}

void Photos::showNextPhoto()
{
    if (m_pixmaps.isEmpty()) return;

    m_photoIndex = (m_photoIndex + 1) % m_pixmaps.size();
    m_photoLabel->setPixmap(m_pixmaps[m_photoIndex]);
}

