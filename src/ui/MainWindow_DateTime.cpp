#include "MainWindow.hpp"
#include <QTimer>
#include <QDateTime>

void MainWindow::setupDateTimeWidget()
{
    m_dateLabel = new QLabel(this);
    m_dateLabel->setGeometry(848, 36, 150, 33);
    m_dateLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    m_dateLabel->setStyleSheet("font-size:18px; font-weight:bold; color:black; background:transparent;");

    m_timeLabel = new QLabel(this);
    m_timeLabel->setGeometry(848, 66, 150, 33);
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    m_timeLabel->setStyleSheet("font-size:15px; color:black; background:transparent;");

    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, this, [this]() {
        QDateTime now = QDateTime::currentDateTime();
        m_dateLabel->setText(now.toString("dd-MM-yyyy"));
        m_timeLabel->setText(now.toString("HH:mm:ss"));
    });
    m_timeTimer->start(1000);
}
