#include "DateTime.hpp"
#include <QDateTime>

DateTime::DateTime(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(200, 70);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setGeometry(0, 0, width(), 30);
    m_dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_dateLabel->setStyleSheet("font-size:18px; font-weight:bold; color:black;");

    m_timeLabel = new QLabel(this);
    m_timeLabel->setGeometry(0, 25, width(), 30);
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_timeLabel->setStyleSheet("font-size:15px; color:black;");

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DateTime::updateDateTime);
    m_timer->start(1000);

    updateDateTime();
}

void DateTime::setPosition(int x, int y)
{
    move(x, y);
    show();
    raise();
}

void DateTime::setColor(const QString &color)
{
    m_dateLabel->setStyleSheet(QString("font-size:18px; font-weight:bold; color:%1;").arg(color));
    m_timeLabel->setStyleSheet(QString("font-size:15px; color:%1;").arg(color));
}

void DateTime::setFontSize(int dateSize, int timeSize)
{
    m_dateLabel->setStyleSheet(QString("font-size:%1px; font-weight:bold; color:black;").arg(dateSize));
    m_timeLabel->setStyleSheet(QString("font-size:%1px; color:black;").arg(timeSize));
}

void DateTime::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    m_dateLabel->setText(now.toString("dd-MM-yyyy"));
    m_timeLabel->setText(now.toString("HH:mm:ss"));
}

