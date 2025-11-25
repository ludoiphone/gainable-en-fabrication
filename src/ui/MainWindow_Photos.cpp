#include "MainWindow.hpp"
#include <QPixmap>
#include <QDir>
#include <QImageReader>
#include <QImage>
#include <QPainter>
#include <QPainterPath>

void MainWindow::setupPhotos()
{
    const int labelW = 400;
    const int labelH = 400;
    const int marginRight = 26;

    int x = 1024 - labelW - marginRight; // aligné avec bouton Stop
    int y = (600 - labelH) / 2;

    m_photoLabel = new QLabel(this);
    m_photoLabel->setGeometry(x, y, labelW, labelH);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setScaledContents(false);
    m_photoLabel->hide();

    QString folder = "/home/ludo/Desktop/test4/photos";
    QDir dir(folder);
    QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
    QStringList files = dir.entryList(filters, QDir::Files);

    for (const QString &f : files) {
        QString path = folder + "/" + f;

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
            path.addRoundedRect(0, 0, labelW, labelH, 30, 30); // rayon 30 px
            painter.setClipPath(path);
            painter.drawImage(0, 0, img);

            m_pixmaps.append(rounded);
        }
    }

    if (!m_pixmaps.isEmpty()) {
        m_photoLabel->setPixmap(m_pixmaps[0]);
        m_photoLabel->show();
    }

    m_photoTimer = new QTimer(this);

    qDebug() << "[Photos] setupPhotos() terminé, nb pixmaps =" << m_pixmaps.size();
}

void MainWindow::updatePhotos()
{
    if (m_pixmaps.isEmpty())
        return;

    m_photoIndex = (m_photoIndex + 1) % m_pixmaps.size();
    m_photoLabel->setPixmap(m_pixmaps[m_photoIndex]);
}
