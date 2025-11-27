#pragma once

#include <QWidget>
#include <QLabel>
#include <QTimer>

class DateTime : public QWidget
{
    Q_OBJECT

public:
    explicit DateTime(QWidget *parent = nullptr);

    void setPosition(int x, int y);
    void setColor(const QString &color);
    void setFontSize(int dateSize, int timeSize);

private slots:
    void updateDateTime();

private:
    QLabel *m_dateLabel;
    QLabel *m_timeLabel;
    QTimer  *m_timer;
};
